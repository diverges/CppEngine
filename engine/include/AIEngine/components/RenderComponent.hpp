/**
 * RenderComponent.hpp - Render Component Declaration
 *
 * Provides visual rendering capability for scene objects including
 * mesh references, material properties, visibility control, and
 * rendering state management.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include "../core/Component.hpp"
#include <string>
#include <cstdint>

namespace AIEngine
{

    // Forward declarations
    class Renderer;
    class Mesh;
    class Material;

    /**
     * @brief Render Component for Visual Objects
     *
     * Manages the visual representation of scene objects by storing
     * references to mesh geometry, materials, and rendering properties.
     * Integrates with the rendering pipeline to draw objects each frame.
     *
     * Features:
     * - Mesh reference management
     * - Visibility control (show/hide)
     * - Material and texture assignment
     * - Rendering layer/order control
     * - Culling and LOD support
     * - Animation and shader state management
     *
     * @example Basic Render Component Usage
     * @code
     * // Add render component to scene node
     * auto* render = node->AddComponent<RenderComponent>();
     *
     * // Set mesh (assume mesh is loaded)
     * render->SetMeshId("cube_mesh");
     *
     * // Configure visibility and properties
     * render->SetVisible(true);
     * render->SetRenderLayer(1);
     *
     * // Set material properties
     * render->SetColor(1.0f, 0.5f, 0.2f); // Orange color
     * render->SetAlpha(0.8f); // Semi-transparent
     *
     * // Component will automatically render during scene render phase
     * @endcode
     */
    class RenderComponent : public Component<RenderComponent>
    {
    public:
        /**
         * @brief Construct render component with default settings
         */
        RenderComponent();

        /**
         * @brief Construct render component with mesh ID
         * @param meshId Identifier for mesh to render
         */
        explicit RenderComponent(const std::string &meshId);

        /**
         * @brief Virtual destructor
         */
        virtual ~RenderComponent() = default;

        // --- Mesh Management ---

        /**
         * @brief Set mesh to render by ID/name
         * @param meshId Identifier for mesh resource
         */
        void SetMeshId(const std::string &meshId);

        /**
         * @brief Get current mesh ID
         * @return Mesh identifier string
         */
        const std::string &GetMeshId() const { return m_meshId; }

        /**
         * @brief Check if component has a valid mesh assigned
         * @return true if mesh ID is set and non-empty
         */
        bool HasMesh() const { return !m_meshId.empty(); }

        /**
         * @brief Clear mesh assignment
         */
        void ClearMesh() { m_meshId.clear(); }

        // --- Visibility Control ---

        /**
         * @brief Set visibility state
         * @param visible true to show, false to hide
         */
        void SetVisible(bool visible);

        /**
         * @brief Check if object is visible
         * @return true if object should be rendered
         */
        bool IsVisible() const { return m_visible; }

        /**
         * @brief Toggle visibility state
         */
        void ToggleVisibility() { m_visible = !m_visible; }

        // --- Rendering Properties ---

        /**
         * @brief Set rendering layer/order
         * @param layer Layer number (higher = rendered later)
         */
        void SetRenderLayer(int layer);

        /**
         * @brief Get rendering layer
         * @return Layer number
         */
        int GetRenderLayer() const { return m_renderLayer; }

        /**
         * @brief Set object color tint
         * @param r Red component (0.0-1.0)
         * @param g Green component (0.0-1.0)
         * @param b Blue component (0.0-1.0)
         */
        void SetColor(float r, float g, float b);

        /**
         * @brief Set object color using packed values
         * @param color Array of RGB values [r, g, b]
         */
        void SetColor(const float color[3]);

        /**
         * @brief Get object color
         * @param color Output array for RGB values [r, g, b]
         */
        void GetColor(float color[3]) const;

        /**
         * @brief Set alpha (transparency) value
         * @param alpha Alpha value (0.0=transparent, 1.0=opaque)
         */
        void SetAlpha(float alpha);

        /**
         * @brief Get alpha value
         * @return Current alpha value
         */
        float GetAlpha() const { return m_alpha; }

        /**
         * @brief Check if object is transparent
         * @return true if alpha < 1.0
         */
        bool IsTransparent() const { return m_alpha < 1.0f; }

        // --- Material Properties ---

        /**
         * @brief Set material ID for advanced rendering
         * @param materialId Identifier for material resource
         */
        void SetMaterialId(const std::string &materialId);

        /**
         * @brief Get current material ID
         * @return Material identifier string
         */
        const std::string &GetMaterialId() const { return m_materialId; }

        /**
         * @brief Check if component has material assigned
         * @return true if material ID is set
         */
        bool HasMaterial() const { return !m_materialId.empty(); }

        /**
         * @brief Set texture ID for simple rendering
         * @param textureId Identifier for texture resource
         */
        void SetTextureId(const std::string &textureId);

        /**
         * @brief Get texture ID
         * @return Texture identifier string
         */
        const std::string &GetTextureId() const { return m_textureId; }

        /**
         * @brief Check if component has texture assigned
         * @return true if texture ID is set
         */
        bool HasTexture() const { return !m_textureId.empty(); }

        // --- Culling and Optimization ---

        /**
         * @brief Enable/disable frustum culling
         * @param enable true to enable culling optimization
         */
        void SetCullingEnabled(bool enable) { m_cullingEnabled = enable; }

        /**
         * @brief Check if frustum culling is enabled
         * @return true if culling is enabled
         */
        bool IsCullingEnabled() const { return m_cullingEnabled; }

        /**
         * @brief Set whether object casts shadows
         * @param castShadows true to cast shadows
         */
        void SetCastShadows(bool castShadows) { m_castShadows = castShadows; }

        /**
         * @brief Check if object casts shadows
         * @return true if shadow casting is enabled
         */
        bool GetCastShadows() const { return m_castShadows; }

        /**
         * @brief Set whether object receives shadows
         * @param receiveShadows true to receive shadows
         */
        void SetReceiveShadows(bool receiveShadows) { m_receiveShadows = receiveShadows; }

        /**
         * @brief Check if object receives shadows
         * @return true if shadow receiving is enabled
         */
        bool GetReceiveShadows() const { return m_receiveShadows; }

        // --- Component Interface ---

        /**
         * @brief Called when component is attached to scene node
         * @param owner The scene node this component belongs to
         */
        void OnAttach(SceneNode *owner) override;

        /**
         * @brief Called each frame for component updates
         * @param deltaTime Time elapsed since last frame
         */
        void OnUpdate(double deltaTime) override;

        /**
         * @brief Called during rendering phase
         * @param renderer The active renderer for drawing operations
         */
        void OnRender(Renderer *renderer) override;

        // --- Utility Methods ---

        /**
         * @brief Reset render component to default state
         */
        void Reset();

        /**
         * @brief Copy render settings from another component
         * @param other Render component to copy from
         */
        void CopyFrom(const RenderComponent &other);

        /**
         * @brief Check if component is ready for rendering
         * @return true if component has mesh and is visible
         */
        bool IsReadyToRender() const;

        /**
         * @brief Get string representation for debugging
         * @return String containing render component state
         */
        std::string ToString() const;

        // --- Performance Tracking ---

        /**
         * @brief Get number of times this component was rendered
         * @return Render count since last reset
         */
        uint32_t GetRenderCount() const { return m_renderCount; }

        /**
         * @brief Reset render statistics
         */
        void ResetRenderStats() { m_renderCount = 0; }

    private:
        // Core rendering data
        std::string m_meshId;     ///< Mesh resource identifier
        std::string m_materialId; ///< Material resource identifier
        std::string m_textureId;  ///< Texture resource identifier

        // Visibility and state
        bool m_visible = true; ///< Visibility flag
        int m_renderLayer = 0; ///< Rendering layer/order

        // Color and transparency
        float m_color[3] = {1.0f, 1.0f, 1.0f}; ///< RGB color tint
        float m_alpha = 1.0f;                  ///< Alpha transparency

        // Rendering options
        bool m_cullingEnabled = true; ///< Enable frustum culling
        bool m_castShadows = true;    ///< Cast shadows flag
        bool m_receiveShadows = true; ///< Receive shadows flag

        // Statistics
        mutable uint32_t m_renderCount = 0; ///< Render call counter

        /**
         * @brief Internal rendering implementation
         * @param renderer Active renderer instance
         */
        void PerformRender(Renderer *renderer);

        /**
         * @brief Check if object should be culled from rendering
         * @param renderer Active renderer for culling tests
         * @return true if object should be skipped
         */
        bool ShouldCull(Renderer *renderer) const;
    };

} // namespace AIEngine