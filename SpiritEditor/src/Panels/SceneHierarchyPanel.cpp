#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "SpiritEngine/Scene/Components.h"
#include <cstring>

/* The Microsoft C++ compiler is non-compliant with the C++ standard and needs
 * the following definition to disable a security warning on std::strncpy().
 */
#ifdef _MSVC_LANG
#define _CRT_SECURE_NO_WARNINGS
#endif

namespace SpiritEngine
{
	extern const std::filesystem::path g_AssetPath;

	bool ColorEdit4(std::string label, float* col)
	{
		ImGui::Text(label.c_str());
		auto& lb = "##" + label;
		return ImGui::ColorEdit4(lb.c_str(), col);
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	Entity SceneHierarchyPanel::GetSelectionContext()
	{
		return m_SelectionContext;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Hierarchy");

		m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID , m_Context.get() };
				DrawEntityNode(entity);
			});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectionContext = {};

		// Right-click on blank space
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			EntityCreation();

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Inspector");

		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::EntityCreation()
	{
		if (ImGui::MenuItem("Create Empty Entity"))
			m_SelectionContext = m_Context->CreateEntity("Empty Entity");

		if (ImGui::MenuItem("Create Square Entity"))
		{
			Entity square = m_Context->CreateEntity("Square Entity");
			auto& spriteRenderer = square.AddComponent<SpriteRendererComponent>();
			auto& collider = square.AddComponent<BoxCollider2DComponent>();
			auto& rigidbody = square.AddComponent<Rigidbody2DComponent>();
			m_SelectionContext = square;
		}

		if (ImGui::MenuItem("Create Circle Entity"))
		{
			Entity circle = m_Context->CreateEntity("Circle Entity");
			auto& circleRenderer = circle.AddComponent<CircleRendererComponent>();
			auto& collider = circle.AddComponent<CircleCollider2DComponent>();
			auto& rigidbody = circle.AddComponent<Rigidbody2DComponent>();
			m_SelectionContext = circle;
		}

		if (ImGui::MenuItem("Create Cube Entity"))
		{
			Entity cube = m_Context->CreateEntity("Cube Entity");
			auto& meshRenderer = cube.AddComponent<MeshRendererComponent>();
			meshRenderer.Mesh = MeshRendererComponent::MeshType::Cube;
			// TODO: Add 3D physics
			// auto& collider = mesh.AddComponent<BoxColliderComponent>();
			// auto& rigidbody = mesh.AddComponent<RigidbodyComponent>();
			m_SelectionContext = cube;
		}

		if (ImGui::MenuItem("Create Camera Entity"))
		{
			Entity camera = m_Context->CreateEntity("Camera Entity");
			camera.AddComponent<CameraComponent>();
			m_SelectionContext = camera;
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& name = entity.GetComponent<TagComponent>().Name;
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
				entityDeleted = true;

			if (ImGui::MenuItem("Duplicate"))
				SetSelectedEntity(m_Context->DuplicateEntity(m_SelectionContext));

			ImGui::Separator();

			EntityCreation();

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, name.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	static void DrawVec2Control(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar(
			);
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
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

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& name = entity.GetComponent<TagComponent>().Name;
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char nameBuffer[256];
			char tagBuffer[256];
			memset(nameBuffer, 0, sizeof(nameBuffer));
			memset(tagBuffer, 0, sizeof(tagBuffer));
			std::strncpy(nameBuffer, name.c_str(), sizeof(nameBuffer));
			std::strncpy(tagBuffer, tag.c_str(), sizeof(tagBuffer));

			ImGui::PushItemWidth(-1);
			if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
			{
				name = std::string(nameBuffer);
			}
			ImGui::PopItemWidth();

			if (ImGui::InputText("##Tag", tagBuffer, sizeof(tagBuffer)))
			{
				tag = std::string(tagBuffer);
			}

			ImGui::SameLine();

			if (ImGui::Button("Add Component", ImVec2(-1, 0)))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent"))
			{
				DisplayAddComponentEntry<CameraComponent>("Camera");
				DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
				DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
				DisplayAddComponentEntry<MeshRendererComponent>("Mesh Renderer");
				DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
				DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
				DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");

				ImGui::EndPopup();
			}

			DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
				{
					DrawVec3Control("Translation", component.Translation);
					glm::vec3 rotation = glm::degrees(component.Rotation);
					DrawVec3Control("Rotation", rotation);
					component.Rotation = glm::radians(rotation);
					DrawVec3Control("Scale", component.Scale, 1.0f);
				});

			DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
				{
					auto& camera = component.Camera;

					ImGui::Checkbox("Primary", &component.Primary);

					const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
					const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
					if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
					{
						for (int i = 0; i < 3; i++)
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
						float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
						if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
							camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

						float perspectiveNear = camera.GetPerspectiveNearClip();
						if (ImGui::DragFloat("Near", &perspectiveNear))
							camera.SetPerspectiveNearClip(perspectiveNear);

						float perspectiveFar = camera.GetPerspectiveFarClip();
						if (ImGui::DragFloat("Far", &perspectiveFar))
							camera.SetPerspectiveFarClip(perspectiveFar);
					}

					if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
					{
						float orthoSize = camera.GetOrthographicSize();
						if (ImGui::DragFloat("Size", &orthoSize))
							camera.SetOrthographicSize(orthoSize);

						float orthoNear = camera.GetOrthographicNearClip();
						if (ImGui::DragFloat("Near", &orthoNear))
							camera.SetOrthographicNearClip(orthoNear);

						float orthoFar = camera.GetOrthographicFarClip();
						if (ImGui::DragFloat("Far", &orthoFar))
							camera.SetOrthographicFarClip(orthoFar);

						ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
					}
				});

			DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
				{
					ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
					DrawVec2Control("UV Offset", component.TextureUVOffset);

					if (ImGui::ImageButton("Texture", ImVec2(100.0f, 100.0f)))
					{
						Ref<Texture2D> whiteTexture = Texture2D::Create(1, 1);
						uint32_t whiteTextureData = 0xffffffff;
						whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
						component.Texture = whiteTexture;
					}
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
							Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
							if (texture->IsLoaded())
								component.Texture = texture;
							else
								SPIRIT_WARN("Could not load texture {0}", texturePath.filename().string());
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
				});

			DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
				{
					ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
					ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
					ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
				});

			DrawComponent<MeshRendererComponent>("Mesh Renderer", entity, [](auto& component)
				{
					// TODO: Add more mesh types
					const char* meshTypeStrings[] = { "Cube" };
					const char* currentMeshTypeString = meshTypeStrings[(int)component.Mesh];
					if (ImGui::BeginCombo("Mesh", currentMeshTypeString))
					{
						for (int i = 0; i < 1; i++)
						{
							bool isSelected = currentMeshTypeString == meshTypeStrings[i];
							if (ImGui::Selectable(meshTypeStrings[i], isSelected))
							{
								currentMeshTypeString = meshTypeStrings[i];
								component.Mesh = (MeshRendererComponent::MeshType)i;
							}

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}

					ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
					DrawVec2Control("UV Offset", component.TextureUVOffset);

					if (component.Mesh == MeshRendererComponent::MeshType::Cube)
					{
						// TODO: Add cubemaps
						if (ImGui::ImageButton("Texture", ImVec2(100.0f, 100.0f)))
						{
							// TODO: Make this load a white cubemap texture with shading
							Ref<Texture2D> whiteTexture = Texture2D::Create(1, 1);
							uint32_t whiteTextureData = 0xffffffff;
							whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
							component.Texture = whiteTexture;
						}
					}
					else
					{
						if (ImGui::ImageButton("Texture", ImVec2(100.0f, 100.0f)))
						{
							Ref<Texture2D> whiteTexture = Texture2D::Create(1, 1);
							uint32_t whiteTextureData = 0xffffffff;
							whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
							component.Texture = whiteTexture;
						}
					}
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
							Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
							if (texture->IsLoaded())
								component.Texture = texture;
							else
								SPIRIT_WARN("Could not load texture {0}", texturePath.filename().string());
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
				});

			DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
				{
					const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
					const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
					if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
					{
						for (int i = 0; i < 3; i++)
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

					ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
				});

			DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
				{
					ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
					ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
					ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
				});

			DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
				{
					ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
					ImGui::DragFloat("Radius", &component.Radius);
					ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
				});
		}

	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

}