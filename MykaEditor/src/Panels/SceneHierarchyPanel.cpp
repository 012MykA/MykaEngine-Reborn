#include "mykapch.hpp"
#include "SceneHierarchyPanel.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

namespace Myka
{
    template <typename T, typename UIFunction>
    static void DrawComponent(const std::string &name, Entity entity, UIFunction uiFunction)
    {
        ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;
        treeNodeFlags |= ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
        treeNodeFlags |= ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (entity.HasComponent<T>())
        {
            auto &component = entity.GetComponent<T>();
            ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
            float lineHeight = ImGui::GetFrameHeight();
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void *)typeid(T).hash_code(), treeNodeFlags, name.c_str());
            ImGui::PopStyleVar();
            ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
            if (ImGui::Button("+", ImVec2(lineHeight, lineHeight)))
            {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (ImGui::MenuItem("Remove Component"))
                    removeComponent = true;

                ImGui::EndPopup();
            }

            if (open)
            {
                uiFunction(component);
                ImGui::TreePop();
            }

            if (removeComponent)
                entity.RemoveComponent<T>();
        }
    }

    template <typename T, typename F>
    static void DrawDragDropResource(const std::string &name,
                                     T &componentField,
                                     std::initializer_list<std::string> validExtensions,
                                     F &&createFunc)
    {
        ImGui::Button(name.c_str(), ImVec2(100.0f, 0.0f));
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const wchar_t *path = static_cast<const wchar_t *>(payload->Data);
                std::filesystem::path resourcePath(path);
                std::string ext = resourcePath.extension().string();

                bool isValid = false;
                for (const auto &validExt : validExtensions)
                {
                    if (ext == validExt)
                    {
                        isValid = true;
                        break;
                    }
                }

                if (isValid)
                {
                    auto newResource = createFunc(resourcePath);
                    componentField = newResource;
                }
                else
                {
                    std::string expected;
                    for (auto v : validExtensions)
                        expected += v + " ";
                    MYKA_CORE_WARN("Invalid format: {0}. Expected: {1}", ext, expected);
                }
            }
            ImGui::EndDragDropTarget();
        }
    }

    static void DrawVec3Control(const std::string &label, glm::vec3 &values, float resetValue = 0.0f, float columnWidth = 100.0f)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopStyleColor(3);
        ImGui::PopFont();

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopStyleColor(3);
        ImGui::PopFont();

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::Columns(1);
        ImGui::Spacing();

        ImGui::PopID();
    }

    static void DrawNode(const Model::Node &node)
    {
        if (ImGui::TreeNode(node.Name.c_str()))
        {
            ImGui::Text("Triangles: %d", node._Mesh->GetIndexCount() / 3);

            auto &material = node._Material;
            ImGui::ColorEdit4("Color", glm::value_ptr(material->AlbedoColor));
            // TODO: AlbedoColorTexture DragDrop

            ImGui::DragFloat("Metallic", &material->Metallic, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Roughness", &material->Roughness, 0.01f, 0.0f, 1.0f);
            // TODO: MetallicRoughnessTexture DragDrop

            ImGui::Text("Child Node Count: %d", node.Children.size());

            for (const auto &childNode : node.Children)
            {
                DrawNode(childNode);
            }

            ImGui::TreePop();
        }
    };

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &scene)
    {
        SetContext(scene);
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene> &context)
    {
        m_Context = context;
        m_SelectionContext = {};
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene");

        auto view = m_Context->m_Registry.view<entt::entity>();
        for (auto entity : view)
        {
            DrawEntityNode(Entity(entity, m_Context.get()));
        }

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        {
            m_SelectionContext = {};
        }

        if (ImGui::BeginPopupContextWindow("##", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("Create Empty Entity"))
            {
                m_Context->CreateEntity("Empty Entity");
            }

            ImGui::EndPopup();
        }

        ImGui::End(); // Scene Hierarchy

        ImGui::Begin("Properties");
        if (m_SelectionContext)
        {
            DrawComponents(m_SelectionContext);
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
    {
        m_SelectionContext = entity;
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto &tag = entity.GetComponent<TagComponent>().Tag;

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

        bool opened = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, flags, tag.c_str());
        if (ImGui::IsItemClicked())
        {
            m_SelectionContext = entity;
        }

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Emtity"))
            {
                entityDeleted = true;
            }

            ImGui::EndPopup();
        }

        if (opened)
            ImGui::TreePop();

        if (entityDeleted)
        {
            m_Context->DestroyEntity(entity);
            if (m_SelectionContext == entity)
                m_SelectionContext = {};
        }
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())
        {

            auto &tag = entity.GetComponent<TagComponent>().Tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, tag.c_str());

            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("Add Component");

        if (ImGui::BeginPopup("Add Component"))
        {
            if (!m_SelectionContext.HasComponent<PointLightComponent>() &&
                !m_SelectionContext.HasComponent<DirectionalLightComponent>() &&
                !m_SelectionContext.HasComponent<SpotLightComponent>())
            {
                if (ImGui::BeginMenu("Light"))
                {
                    if (ImGui::MenuItem("Point Light"))
                    {
                        m_SelectionContext.AddComponent<PointLightComponent>();
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::MenuItem("Directional Light"))
                    {
                        m_SelectionContext.AddComponent<DirectionalLightComponent>();
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::MenuItem("Spot Light"))
                    {
                        m_SelectionContext.AddComponent<SpotLightComponent>();
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndMenu();
                }
            }

            if (!m_SelectionContext.HasComponent<SkyLightComponent>())
            {
                if (ImGui::MenuItem("Skybox"))
                {
                    m_SelectionContext.AddComponent<SkyLightComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::Separator();

            if (!m_SelectionContext.HasComponent<MeshComponent>())
            {
                if (ImGui::MenuItem("Mesh"))
                {
                    m_SelectionContext.AddComponent<MeshComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }

            if (!m_SelectionContext.HasComponent<MaterialComponent>())
            {
                if (ImGui::MenuItem("Material"))
                {
                    m_SelectionContext.AddComponent<MaterialComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }

            if (!m_SelectionContext.HasComponent<ModelComponent>())
            {
                if (ImGui::MenuItem("Model"))
                {
                    m_SelectionContext.AddComponent<ModelComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }

            if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
            {
                if (ImGui::MenuItem("Sprite Renderer"))
                {
                    m_SelectionContext.AddComponent<SpriteRendererComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }

            if (!m_SelectionContext.HasComponent<CircleRendererComponent>())
            {
                if (ImGui::MenuItem("Circle Renderer"))
                {
                    m_SelectionContext.AddComponent<CircleRendererComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::Separator();

            if (!m_SelectionContext.HasComponent<Rigidbody2DComponent>())
            {
                if (ImGui::MenuItem("Rigidbody 2D"))
                {
                    m_SelectionContext.AddComponent<Rigidbody2DComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }

            if (!m_SelectionContext.HasComponent<BoxCollider2DComponent>())
            {
                if (ImGui::MenuItem("Box Collider 2D"))
                {
                    m_SelectionContext.AddComponent<BoxCollider2DComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }

            if (!m_SelectionContext.HasComponent<CircleCollider2DComponent>())
            {
                if (ImGui::MenuItem("Circle Collider 2D"))
                {
                    m_SelectionContext.AddComponent<CircleCollider2DComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::Separator();

            if (!m_SelectionContext.HasComponent<CameraComponent>())
            {
                if (ImGui::MenuItem("Camera"))
                {
                    m_SelectionContext.AddComponent<CameraComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
        ImGui::PopItemWidth();

        DrawComponent<TransformComponent>("Transform", entity, [](auto &component)
                                          {
            DrawVec3Control("Position", component.Position);
                glm::vec3 rotation = glm::degrees(component.Rotation);
                DrawVec3Control("Rotation", rotation);
                component.Rotation = glm::radians(rotation);

                DrawVec3Control("Scale", component.Scale, 1.0f); });

        DrawComponent<PointLightComponent>("Point Light", entity, [](auto &component)
                                           {
            ImGui::ColorEdit3("Color", glm::value_ptr(component.Color));
            // Интенсивность не может быть отрицательной
            ImGui::DragFloat("Intensity", &component.Intensity, 0.1f, 0.0f, 1000.0f);
            // Радиус должен быть больше нуля
            ImGui::DragFloat("Radius", &component.Radius, 0.1f, 0.01f, 100.0f);
            ImGui::DragFloat("Falloff", &component.Falloff, 0.05f, 0.0f, 10.0f); });

        DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](auto &component)
                                                 {
            ImGui::ColorEdit3("Color", glm::value_ptr(component.Color));
            ImGui::DragFloat("Intensity", &component.Intensity, 0.1f, 0.0f, 100.0f);
            ImGui::Checkbox("Cast Shadows", &component.CastShadows); });

        DrawComponent<SpotLightComponent>("Spot Light", entity, [](auto &component)
                                          {
            ImGui::ColorEdit3("Color", glm::value_ptr(component.Color));
            ImGui::DragFloat("Intensity", &component.Intensity, 0.1f, 0.0f, 1000.0f);
            ImGui::DragFloat("Range", &component.Range, 0.1f, 0.01f, 500.0f);

            if (ImGui::DragFloat("InnerCutoff", &component.InnerCutoff, 0.5f, 0.0f, component.OuterCutoff))
            {
                if (component.InnerCutoff > component.OuterCutoff)
                    component.InnerCutoff = component.OuterCutoff;
            }

            if (ImGui::DragFloat("OuterCutoff", &component.OuterCutoff, 0.5f, component.InnerCutoff, 90.0f))
            {
                if (component.OuterCutoff < component.InnerCutoff)
                    component.OuterCutoff = component.InnerCutoff;
            } });

        // DrawComponent<SkyLightComponent>("Skybox", entity, [](auto &component)
        //                                  {
        //     ImGui::DragFloat("Intensity", &component.Intensity, 0.1f, 0.0f, 10.0f);

        //     DrawDragDropResource("HDR Panorama", component.EnvironmentMap, { ".hdr" }, 
        //     [](const std::filesystem::path& path)
        //     {
        //         auto panorama = Texture2D::Create(path);
                
        //         TextureSpecification spec;
        //         spec.Width = 1024; spec.Height = 1024;
        //         spec.Format = ImageFormat::RGBA32F;
        //         spec.GenerateMips = true;
        //         auto envMap = CubeTexture::Create(spec);

        //         // IBLBaker::ConvertPanoramaToCubemap(panorama, envMap);
                
        //         return envMap;
        //     }); });

        DrawComponent<MeshComponent>("Mesh", entity, [](auto &component)
                                     {
            if (component._Mesh)
            {
                ImGui::Text("Triangles %d", component._Mesh->GetIndexCount() / 3);
            } });

        DrawComponent<MaterialComponent>("Material", entity, [](auto &component)
                                         {
            auto& material = component._Material;
            if (material)
            {
                ImGui::ColorEdit4("Color", glm::value_ptr(material->AlbedoColor));
                // TODO: AlbedoColorTexture DragDrop
    
                ImGui::DragFloat("Metallic", &material->Metallic, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Roughness", &material->Roughness, 0.01f, 0.0f, 1.0f);
                // TODO: MetallicRoughnessTexture DragDrop
            } });

        DrawComponent<ModelComponent>("Model", entity, [](auto &component)
                                      {
            DrawDragDropResource("Model", component._Model, {".gltf", ".glb"}, [](auto p){ return Model::Create(p); });

            if (component._Model)
            {
                if (ImGui::CollapsingHeader("Nodes"))
                {
                    uint32_t nodeCount = 0;
                    for (const auto& node : component._Model->GetNodes())
                    {
                        std::string nodeID = "nodeID##" + std::to_string(nodeCount);
                        ImGui::PushID(nodeID.c_str());

                        DrawNode(node);

                        ImGui::PopID();
                        nodeCount++;
                    }
                }
            } });

        DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto &component)
                                               {
            DrawDragDropResource("Texture", component.Texture, {".hdr", ".png", ".jpg", "jpeg"}, [](auto p){ return Texture2D::Create(p); });

            ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

            ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 1.0f, 100.0f); });

        DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto &component)
                                               {
            ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
            ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
            ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f); });

        DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto &component)
                                            {
            const char *bodyTypeStrings[] = {"Static", "Kinematic", "Dynamic"};
            const char *currentBodyTypeString = bodyTypeStrings[(int)component.Type];
            if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
            {
                for (int i = 0; i < 3; ++i)
                {
                    bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
                    if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
                    {
                        currentBodyTypeString = bodyTypeStrings[i];
                        component.Type = (Rigidbody2DComponent::BodyType)i;
                    }
                    
                    if (isSelected)
                    ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            
            ImGui::Checkbox("Fixed Rotation", &component.FixedRotation); });

        DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto &component)
                                              {
            ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
            ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
            ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f); });

        DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto &component)
                                                 {
            ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
            ImGui::DragFloat("Radius", &component.Radius, 0.1f, 0.0f, 0.0f);
            ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f); });

        DrawComponent<CameraComponent>("Camera", entity, [](auto &component)
                                       {
            auto &camera = component .Camera;
                
            ImGui::Checkbox("Primary", &component.Primary);
            
            const char *projectionTypeStrings[] = {"Perspective", "Orthographic"};
            const char *currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
            if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
            {
                for (int i = 0; i < 2; ++i)
                {
                    bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                    if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
                    {
                        currentProjectionTypeString = projectionTypeStrings[i];
                        camera.SetProjectionType((SceneCamera::ProjectionType)i);
                    }
                    
                    if (isSelected)
                    ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            
            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
            {
                float perspectiveFOV = glm::degrees(camera.GetPerspectiveFOV());
                if (ImGui::DragFloat("FOV", &perspectiveFOV))
                camera.SetPerspectiveFOV(glm::radians(perspectiveFOV));
                
                float perspectiveNear = camera.GetPerspectiveNear();
                if (ImGui::DragFloat("Near", &perspectiveNear))
                camera.SetPerspectiveNear(perspectiveNear);
                
                float perspectiveFar = camera.GetPerspectiveFar();
                if (ImGui::DragFloat("Far", &perspectiveFar))
                camera.SetPerspectiveFar(perspectiveFar);
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
            {
                float orthoSize = camera.GetOrthographicSize();
                if (ImGui::DragFloat("Size", &orthoSize))
                camera.SetOrthographicSize(orthoSize);
                
                float orthoNear = camera.GetOrthographicNear();
                if (ImGui::DragFloat("Near", &orthoNear))
                camera.SetOrthographicNear(orthoNear);
                
                float orthoFar = camera.GetOrthographicFar();
                if (ImGui::DragFloat("Far", &orthoFar))
                camera.SetOrthographicFar(orthoFar);
                
                ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
            } });
    }
} // namespace Myka