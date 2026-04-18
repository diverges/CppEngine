# Problem Statement

The engine needs a scene representation that is both flexible and cache-efficient. A traditional object-oriented scene graph (one object per node, virtual dispatch, pointer-chasing children) couples data and behaviour, makes it hard to add new capabilities without modifying existing classes, and produces scattered memory access patterns that thrash the CPU cache — especially when many systems need to read the same entities each frame.

The solution is a lightweight Entity Component System (ECS): entities are bare integer IDs, components are plain data structs stored in dense arrays, and systems are the only place that contains logic. This separates concerns cleanly and lets the CPU iterate component data with no gaps and no indirection.

# Proposal

## Core Concepts

- **Entity** — a plain `uint32_t` ID. No methods, no vtable.
- **Component** — a plain data struct (Transform, MeshRenderer, Camera, Hierarchy). No logic.
- **System** — a class with an `Update(Registry&, float dt)` method. All logic lives here.
- **Registry** — owns all component pools and entity lifecycle.

## Component Data

```cpp
using EntityID = uint32_t;
constexpr EntityID NULL_ENTITY = 0;

struct Transform {
    glm::vec3 position {0, 0, 0};
    glm::quat rotation {1, 0, 0, 0};
    glm::vec3 scale    {1, 1, 1};
};

struct WorldTransform {   // computed each frame by TransformSystem
    glm::mat4 matrix {1.f};
};

struct MeshRenderer {
    uint32_t meshId     = 0;
    uint32_t materialId = 0;
    bool     visible    = true;
};

struct Camera {
    float fov      = 60.f;
    float nearClip = 0.1f;
    float farClip  = 1000.f;
};

struct Hierarchy {
    EntityID parent = NULL_ENTITY;
    std::vector<EntityID> children;
};
```

## Scene Hierarchy

```
Root (Hierarchy)
├── Entity 1 — Transform, WorldTransform, Camera
└── Entity 2 — Transform, WorldTransform, MeshRenderer, Hierarchy
    └── Entity 3 — Transform, WorldTransform, MeshRenderer
```

Parent/child relationships are expressed via the `Hierarchy` component, not through node pointers. This keeps the scene data flat.

## Registry API

```cpp
class Registry {
public:
    EntityID Create();
    void     Destroy(EntityID entity);

    template<typename T>
    T&   Emplace(EntityID entity, T component = {});

    template<typename T>
    T*   Get(EntityID entity);      // nullptr if absent

    template<typename T>
    bool Has(EntityID entity) const;

    template<typename T>
    void Remove(EntityID entity);

    // Iterate all entities that have every listed component type
    template<typename... Ts>
    void View(std::invocable<EntityID, Ts&...> auto fn);
};
```

## ComponentPool — Sparse Set Storage

Each component type is backed by a sparse set: a sparse array for O(1) entity lookup and a dense packed array for linear iteration. Removing an entity swaps it with the last element to keep the dense array gap-free.

```
sparse[entityID] → index into dense arrays   (O(1) lookup)
dense_entities[] → packed list of EntityIDs  (linear iteration)
dense_components[] → packed component data   (cache-friendly reads)
```

## View — Cache-Efficient Intersection

`View<Ts...>` drives iteration from the smallest component pool and checks membership in the others via O(1) sparse lookups. All accesses are sequential or L1-cached — the CPU prefetcher can pipeline both dense arrays simultaneously.

```cpp
// Internally: iterate smallest pool, check Has<T>(e) for each other T,
// then call fn(e, pool_A.dense[i], pool_B.dense[j], ...)
reg.View<MeshRenderer, WorldTransform>(
    [](EntityID e, MeshRenderer& mr, WorldTransform& wt) {
        if (mr.visible) Submit(mr.meshId, wt.matrix);
    });
```

This is significantly faster than a flat `T[MAX_ENTITIES]` array when iterating an intersection: a flat array iterated by entity ID produces strided/random access into a second array because the entity IDs of one component set are not contiguous with respect to another.

## System Order

Systems run in a fixed order each frame:

```
1. TransformSystem   — walks Hierarchy, propagates Transform → WorldTransform
2. MeshRenderSystem  — reads WorldTransform + MeshRenderer, submits draw calls
3. (future) PhysicsSystem, AudioSystem, AnimationSystem, ...
```

## Usage Example

```cpp
Registry reg;

// Camera
EntityID cam = reg.Create();
reg.Emplace<Transform>(cam, { .position = {0, 5, -10} });
reg.Emplace<WorldTransform>(cam);
reg.Emplace<Camera>(cam, { .fov = 75.f });

// Cube
EntityID cube = reg.Create();
reg.Emplace<Transform>(cube);
reg.Emplace<WorldTransform>(cube);
reg.Emplace<MeshRenderer>(cube, { .meshId = cubeMeshId });

// Game loop
while (running) {
    float dt = timer.Tick();
    for (auto& sys : systems)
        sys->Update(reg, dt);
}
```

---

# Appendix

## A. Why a flat `T[MAX_ENTITIES]` array is not sufficient

A flat array per component type is fast to iterate *alone*, but the gap and correlation problems appear the moment a system needs two components together.

**Gap / sparsity problem** — destroyed entities leave holes. With 1000 slots but 200 alive entities, 80% of every cache line loaded contains dead data:

```
EntityID:   0    1    2    3    4    5    6    7
Transform: [t0] [t1] [--] [t3] [--] [t5] [--] [t7]
Mesh:      [--] [m1] [--] [m3] [--] [--] [m6] [--]
```

**Correlation / strided-access problem** — even with a contiguous array, iterating one component and indexing into another by entity ID produces random-stride access:

```cpp
for (EntityID e : meshRenderers.GetAllEntities()) {
    Transform& t = transforms[e];   // e values are {1, 3, 6, 97, 203, ...}
    // → non-sequential jumps into the transform array; CPU cannot prefetch
}
```

The sparse-set dense pool eliminates both: only live entities are stored, and all of them are packed with no gaps.

---

## B. `ComponentPool<T>` — Full Implementation

Each component type owns one pool. The sparse set gives O(1) lookup via `sparse[entityID]` and linear iteration via `dense_components[]`. Removal swaps the target with the last element to keep the dense array contiguous.

```cpp
template<typename T>
class ComponentPool {
    static constexpr uint32_t INVALID = UINT32_MAX;

    std::vector<uint32_t>  sparse;           // sparse[entityID] = dense index
    std::vector<EntityID>  dense_entities;   // packed entity IDs
    std::vector<T>         dense_components; // packed component data

public:
    T& Emplace(EntityID e, T component = {}) {
        if (e >= sparse.size())
            sparse.resize(e + 1, INVALID);

        uint32_t idx = (uint32_t)dense_entities.size();
        sparse[e] = idx;
        dense_entities.push_back(e);
        dense_components.push_back(std::move(component));
        return dense_components.back();
    }

    T* Get(EntityID e) {
        if (e >= sparse.size() || sparse[e] == INVALID) return nullptr;
        return &dense_components[sparse[e]];
    }

    bool Has(EntityID e) const {
        return e < sparse.size() && sparse[e] != INVALID;
    }

    void Remove(EntityID e) {
        if (!Has(e)) return;
        uint32_t idx  = sparse[e];
        EntityID last = dense_entities.back();

        dense_entities[idx]   = last;
        dense_components[idx] = std::move(dense_components.back());
        sparse[last]          = idx;
        sparse[e]             = INVALID;

        dense_entities.pop_back();
        dense_components.pop_back();
    }

    size_t   size()             const { return dense_components.size(); }
    EntityID entity_at(size_t i) const { return dense_entities[i]; }
};
```

---

## C. `View<Ts...>` — Full Implementation

`View` selects the smallest pool to drive the outer loop, then checks membership in all other pools with O(1) sparse lookups. A C++17 fold expression short-circuits on the first missing component.

```cpp
template<typename... Ts>
void Registry::View(std::invocable<EntityID, Ts&...> auto fn) {
    // 1. Pick the smallest pool to minimise outer iterations
    size_t sizes[]  = { GetPool<Ts>().size()... };
    size_t minIdx   = std::min_element(std::begin(sizes), std::end(sizes))
                      - std::begin(sizes);

    // Helper: iterate the chosen pool by its runtime index
    auto dispatch = [&]<size_t I>() {
        auto& driving = std::get<I>(std::tie(GetPool<Ts>()...));
        for (size_t i = 0; i < driving.size(); ++i) {
            EntityID e = driving.entity_at(i);
            // 2. Check all pools have this entity (fold, short-circuits)
            if (!(GetPool<Ts>().Has(e) && ...)) continue;
            // 3. Deliver refs — all are dense[sparse[e]], no pointer chasing
            fn(e, *GetPool<Ts>().Get(e)...);
        }
    };

    // Select driving pool at compile time via index sequence
    [&]<size_t... Is>(std::index_sequence<Is...>) {
        ((minIdx == Is ? (dispatch.template operator()<Is>(), 0) : 0), ...);
    }(std::index_sequence_for<Ts...>{});
}
```

**What the CPU sees per iteration:**

```
i=0: load driving_pool.dense_entities[0]    ← sequential read (stream A)
     load pool_B.sparse[e]                  ← O(1), stays in L1 all loop
     load pool_B.dense_components[idx]      ← sequential read (stream B)
     call fn(...)
i=1: advance both dense arrays by 1 element — prefetcher handles both streams
```

Both `dense_components` streams are sequential; the sparse arrays are small integer arrays that remain in L1 cache for the entire loop. This is why `View` outperforms any approach that indexes into a second array by non-contiguous entity ID.

---

## D. Known `View<Ts...>` Performance Issues and Corrections

A review of the implementation above identified three high-severity issues. They are documented here along with the corrected approach.

### D.1 — Redundant Driving-Pool Check and Double Sparse Lookup

**Problem.** The implementation calls `Has(e)` across the full `Ts...` pack (including the driving pool, which always holds `e`), then calls `Get(e)` again across the full pack to obtain the references. This produces `2 × sizeof...(Ts)` sparse reads per entity per iteration — twice what is necessary, plus one guaranteed-true check every loop.

**Fix.** Replace the `Has + Get` pair with a single `Get` per pool. Cache the returned pointers. Exclude the driving pool index from the null-check fold, since its pointer is always valid.

```cpp
auto dispatch = [&]<size_t DrivingIdx>() {
    auto& driving = std::get<DrivingIdx>(std::tie(GetPool<Ts>()...));
    const size_t count = driving.size();

    for (size_t i = 0; i < count; ++i) {
        EntityID e = driving.entity_at(i);

        // One Get() per pool — single sparse read, pointer cached for fn call.
        auto ptrs = std::make_tuple(GetPool<Ts>().Get(e)...);

        // Null-check all pools except the driving one (always non-null).
        bool valid = [&]<size_t... Js>(std::index_sequence<Js...>) {
            return ((Js == DrivingIdx || std::get<Js>(ptrs) != nullptr) && ...);
        }(std::index_sequence_for<Ts...>{});

        if (!valid) continue;

        fn(e, *std::get<IndexOf<Ts, Ts...>>(ptrs)...);
    }
};
```

**Cost reduction:** `2N` sparse reads → `N − 1` sparse reads per matching entity (where N = `sizeof...(Ts)`).

### D.2 — Secondary Pool Component Access Is Not Sequential

**Problem.** The spec diagram claims both `dense_components` streams are sequential. This is only true for the driving pool. Secondary pools are indexed by `sparse[e]`, where `e` comes from the driving pool's entity list. Because entities were inserted into each pool independently, `sparse[e]` values are arbitrary — the CPU prefetcher cannot predict them. At scale (10k+ entities) this causes L1/L2 thrashing on every secondary component read.

**Fix — Owning Groups.** Declare a Group for component pairs that are always iterated together. The Registry maintains a sorted region `[0, group_size)` in each participating pool such that both pools store the same entity at the same dense index. Both `dense_components` arrays can then be iterated by a single sequential counter with no sparse involvement at all.

```cpp
// Registration (once, before any Emplace for these types)
reg.Group<Transform, WorldTransform>();

// View over a group: no sparse lookups, two sequential streams
for (size_t i = 0; i < group_size; ++i) {
    EntityID e = pool_A.dense_entities[i];
    fn(e, pool_A.dense_components[i],   // stream A — sequential
          pool_B.dense_components[i]);  // stream B — sequential ✓
}
```

`Emplace` and `Remove` perform one extra swap to maintain the group invariant. Reserve groups for the hottest iteration pairs only (e.g. `Transform + WorldTransform`, `WorldTransform + MeshRenderer`).

### D.3 — Unbounded Sparse Array Growth Without Entity ID Recycling

**Problem.** `ComponentPool::Emplace` resizes `sparse` to `entityID + 1`. Without ID recycling, `sparse` grows monotonically with every entity ever created. Once it exceeds L1 size (~32–64 KB) the premise that "sparse arrays stay in L1 all loop" fails, and every `sparse[e]` lookup becomes an L2 or L3 miss.

**Fix.** `Registry::Create` must recycle destroyed entity IDs via a freelist, bounding the sparse array to the peak live entity count rather than the lifetime total.

```cpp
class Registry {
    EntityID             next_id = 1;   // 0 reserved as NULL_ENTITY
    std::vector<EntityID> free_ids;

public:
    EntityID Create() {
        if (!free_ids.empty()) {
            EntityID e = free_ids.back();
            free_ids.pop_back();
            return e;
        }
        return next_id++;
    }

    void Destroy(EntityID e) {
        // ... remove from all component pools ...
        free_ids.push_back(e);
    }
};
```

For games where stale `EntityID` references are a risk, encode a generation counter into the upper bits:

```cpp
// EntityID layout: [16-bit generation | 16-bit index]
// Supports up to 65 535 concurrently live entities.
```

The generation is incremented each time a slot is recycled; a lookup that finds a mismatched generation returns null rather than silently aliasing the new occupant.
