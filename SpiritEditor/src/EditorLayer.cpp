#include "spiritpch.h"

#include "EditorLayer.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <iostream>
#include <fstream>
#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SpiritEngine/Scene/SceneSerializer.h"
#include "SpiritEngine/Utils/PlatformUtils.h"

#include "ImGuizmo.h"

#include "SpiritEngine/Math/Math.h"

#include <Glad/include/glad/glad.h>

namespace SpiritEngine {

	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
	{
	}

	template<typename T>
	void DrawComponent(const std::string& name, Entity entity)
	{
		Entity t = {};
		if (entity != t && entity.HasComponent<T>())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.663f, 0.663f, 0.663f, 1.0f });
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		}
		if (ImGui::MenuItem(name.c_str()))
		{
			entity.AddComponent<T>();

			ImGui::CloseCurrentPopup();
		}
		if (entity != t && entity.HasComponent<T>())
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);
			ImGui::PopStyleColor(1);
		}
	}

	void EditorLayer::OnAttach()
	{
		SPIRIT_PROFILE_FUNCTION();

		m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
		m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_IDFramebuffer = Framebuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(sceneFilePath);
		}
		
		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		m_EditorCamera.m_ViewportFocused = m_ViewportFocused;
		m_EditorCamera.m_ViewportHovered = m_ViewportHovered;

#if 0
		// Entity
		auto square = m_ActiveScene->CreateEntity("Green Square");
		square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

		auto redSquare = m_ActiveScene->CreateEntity("Red Square");
		redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

		m_SquareEntity = square;

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");
		auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
		cc.Primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			virtual void OnCreate() override
			{
				auto& translation = GetComponent<TransformComponent>().Translation;
				translation.x = rand() % 10 - 5.0f;
			}

			virtual void OnDestroy() override
			{
			}

			virtual void OnUpdate(Timestep ts) override
			{
				auto& translation = GetComponent<TransformComponent>().Translation;

				float speed = 5.0f;

				if (Input::IsKeyPressed(Key::A))
					translation.x -= speed * ts;
				if (Input::IsKeyPressed(Key::D))
					translation.x += speed * ts;
				if (Input::IsKeyPressed(Key::W))
					translation.y += speed * ts;
				if (Input::IsKeyPressed(Key::S))
					translation.y -= speed * ts;
			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);


		
		std::ifstream file("preferences.yaml");
		if (file.is_open())
		{
			int i = 0;
			for (string line = ""; std::getline(file, line); i++)
			{
				if (i == 0)
				{
					// ShowPhysicsColliders
					line.erase(line.begin(), line.begin() + 22);

					if (line == "1")
						m_ShowPhysicsColliders = true;
					else if (line == "0")
						m_ShowPhysicsColliders = false;
					else
						m_ShowPhysicsColliders = true;
				}
				else if (i == 1)
				{
					// 3DCamera
					line.erase(line.begin(), line.begin() + 10);

					if (line == "1")
						m_EditorCamera.m_3DCamera = true;
					else if (line == "0")
						m_EditorCamera.m_3DCamera = false;
					else
						m_EditorCamera.m_3DCamera = true;
				}
				else if (i == 2)
				{
					// Theme
					line.erase(line.begin(), line.begin() + 7);

					if (line == "Dark")
						ImGuiLayer::SetDarkThemeColors();
					else if (line == "Light")
						ImGuiLayer::SetLightThemeColors();
					else if (line == "CorporateGrey")
						ImGuiLayer::SetCorporateGreyThemeColors();
					else if (line == "ClassicDark")
						ImGuiLayer::SetClassicDarkThemeColors();
					else if (line == "ClassicLight")
						ImGuiLayer::SetClassicLightThemeColors();
					else if (line == "Classic")
						ImGuiLayer::SetClassicThemeColors();
					else
						ImGuiLayer::SetDarkThemeColors();
				}
			}
			file.close();
		}
		else
		{
			ImGuiLayer::SetDarkThemeColors();
			m_ShowPhysicsColliders = true;
			m_EditorCamera.m_3DCamera = true;
		}
	}

	void EditorLayer::OnDetach()
	{
		SPIRIT_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		SPIRIT_PROFILE_FUNCTION();

		// Resize
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_IDFramebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Render
		Renderer2D::ResetStats();
		Renderer3D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();
		m_Framebuffer->Bind();

		// Clear our entity ID attachment to -1
		m_Framebuffer->ClearAttachment(1, -1);
		
		switch (m_SceneState)
		{
		case SceneState::Edit:
		{
			if (m_ViewportFocused)
				m_CameraController.OnUpdate(ts);

			m_EditorCamera.OnUpdate(ts);

			m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
			break;
		}
		case SceneState::Play:
		{
			m_ActiveScene->OnUpdateRuntime(ts);
			break;
		}
		}

		//SPIRIT_PROFILE_SCOPE("Renderer Draw");
		//Renderer2D::BeginScene(m_CameraController.GetCamera());
		//Renderer3D::BeginScene(m_CameraController.GetCamera());
		//Renderer2D::EndScene();
		//Renderer3D::EndScene();

		auto[mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;
		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y - 30 && m_ViewportHovered && m_ViewportFocused)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mx, my);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}

		OnOverlayRender();

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		SPIRIT_PROFILE_FUNCTION();

		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 396.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();

				if (ImGui::MenuItem("Save", "Ctrl+S", false, m_ActiveScene != nullptr))
					SaveScene();

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S", false, m_ActiveScene != nullptr))
					SaveSceneAs();

				if (ImGui::MenuItem("Exit", "Alt+F4")) Application::Get().Close();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Entity"))
			{
				m_SceneHierarchyPanel.EntityCreation();

				ImGui::EndMenu();
			}
			
			if (ImGui::BeginMenu("Component"))
			{
				Entity t = {};
				if (m_SceneHierarchyPanel.GetSelectionContext() == t)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.663f, 0.663f, 0.663f, 1.0f });
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				}

				DrawComponent<CameraComponent>("Camera", m_SceneHierarchyPanel.GetSelectionContext());
				DrawComponent<SpriteRendererComponent>("Sprite Renderer", m_SceneHierarchyPanel.GetSelectionContext());
				DrawComponent<CircleRendererComponent>("Circle Renderer", m_SceneHierarchyPanel.GetSelectionContext());
				DrawComponent<MeshRendererComponent>("Mesh Renderer", m_SceneHierarchyPanel.GetSelectionContext());
				DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", m_SceneHierarchyPanel.GetSelectionContext());
				DrawComponent<BoxCollider2DComponent>("Box Collider 2D", m_SceneHierarchyPanel.GetSelectionContext());
				DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", m_SceneHierarchyPanel.GetSelectionContext());

				if (m_SceneHierarchyPanel.GetSelectionContext() == t)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);
					ImGui::PopStyleColor(1);
				}


				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel.OnImGuiRender();

		ImGui::Begin("Stats");

		auto stats2d = Renderer2D::GetStats();
		auto stats3d = Renderer3D::GetStats();
		ImGui::Text("Renderer Stats:");
		ImGui::Text("Draw Calls: %d (2D: %d, 3D: %d)", stats2d.DrawCalls + stats3d.DrawCalls, stats2d.DrawCalls, stats3d.DrawCalls);
		ImGui::Text("Quads: %d (2D: %d, 3D: %d)", stats2d.QuadCount + stats3d.QuadCount, stats2d.QuadCount, stats3d.QuadCount);
		ImGui::Text("Vertices: %d (2D: %d, 3D: %d)", stats2d.GetTotalVertexCount() + stats3d.GetTotalVertexCount(), stats2d.GetTotalVertexCount(), stats3d.GetTotalVertexCount());
		ImGui::Text("Indices: %d (2D: %d, 3D: %d)", stats2d.GetTotalIndexCount() + stats3d.GetTotalIndexCount(), stats2d.GetTotalIndexCount(), stats3d.GetTotalIndexCount());

		std::string name = "Null";
		if ((entt::entity)m_HoveredEntity != entt::null)
			name = m_HoveredEntity.GetComponent<TagComponent>().Name;
		ImGui::Text("Hovered Entity: %s", name.c_str());

		ImGui::End();

		ImGui::Begin("Settings");

		std::string out;
		ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);
		out += std::string("ShowPhysicsColliders: ") + std::to_string(m_ShowPhysicsColliders) + std::string("\n");
		ImGui::Checkbox("3D Camera", &m_EditorCamera.m_3DCamera);
		out += std::string("3DCamera: ") + std::to_string(m_EditorCamera.m_3DCamera) + std::string("\n");

		if (ImGui::BeginMenu("Theme"))
		{
			if (ImGui::MenuItem("Dark Theme"))
				ImGuiLayer::SetDarkThemeColors();
			
			if (ImGui::MenuItem("Light Theme"))
				ImGuiLayer::SetLightThemeColors();
			
			if (ImGui::MenuItem("Corporate Grey Theme"))
				ImGuiLayer::SetCorporateGreyThemeColors();
			
			if (ImGui::MenuItem("Classic Dark Theme"))
				ImGuiLayer::SetClassicDarkThemeColors();

			if (ImGui::MenuItem("Classic Light Theme"))
				ImGuiLayer::SetClassicLightThemeColors();

			if (ImGui::MenuItem("Classic Theme"))
				ImGuiLayer::SetClassicThemeColors();

			ImGui::EndMenu();
		}
		out += std::string("Theme: ") + ImGuiLayer::GetTheme() + std::string("\n");

		std::ofstream fout("preferences.yaml");
		fout << out.c_str();

		ImGui::End();
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		m_EditorCamera.m_ViewportFocused = m_ViewportFocused;
		m_EditorCamera.m_ViewportHovered = m_ViewportHovered;

		//Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);
		Application::Get().GetImGuiLayer()->BlockEvents(false);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path parentPath = std::filesystem::path(path).parent_path();
				std::filesystem::path filePath = std::filesystem::path(path);

				if (filePath.extension().string() == ".spirit")
					OpenScene(std::filesystem::path(g_AssetPath) / path);
				if (filePath.extension().string() == ".prefab")
					SPIRIT_WARN("Prefabs aren't supported yet!");
				else if (filePath.extension().string() == ".png")
				{
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
					if (texture->IsLoaded())
					{
						if (m_HoveredEntity && m_HoveredEntity.HasComponent<SpriteRendererComponent>())
							m_HoveredEntity.GetComponent<SpriteRendererComponent>().Texture = texture;
						else if (m_HoveredEntity && m_HoveredEntity.HasComponent<MeshRendererComponent>())
							m_HoveredEntity.GetComponent<MeshRendererComponent>().Texture = texture;
						else if (!m_HoveredEntity)
						{
							Entity sprite = m_ActiveScene->CreateEntity("Sprite");
							auto& spriteRenderer = sprite.AddComponent<SpriteRendererComponent>();
							spriteRenderer.Texture = texture;
							auto& collider = sprite.AddComponent<BoxCollider2DComponent>();
							auto& rigidbody = sprite.AddComponent<Rigidbody2DComponent>();
							m_SceneHierarchyPanel.SetSelectedEntity(sprite);
						}
					}
					else
						SPIRIT_WARN("Could not load texture {0}", texturePath.filename().string());
				}
				// TODO: Add custom models to the Mesh Renderer Component
				/*else if (filePath.extension().string() == ".png")
				{
					std::filesystem::path modelPath = std::filesystem::path(g_AssetPath) / path;
					Ref<Texture2D> model = Texture2D::Create(modelPath.string());
					if (model->IsLoaded())
					{
						if (m_HoveredEntity && m_HoveredEntity.HasComponent<MeshRendererComponent>())
						{
							m_HoveredEntity.GetComponent<MeshRendererComponent>().Mesh = MeshRendererComponent::MeshType::Custom;
							m_HoveredEntity.GetComponent<MeshRendererComponent>().Model = model;
						}
						else if (!m_HoveredEntity)
						{
							Entity mesh = m_ActiveScene->CreateEntity("Mesh");
							auto& meshRenderer = mesh.AddComponent<MeshRendererComponent>();
							meshRenderer.Mesh = MeshRendererComponent::MeshType::Custom;
							meshRenderer.Model = model;
							// TODO: Add 3D physics
							// auto& collider = mesh.AddComponent<MeshColliderComponent>();
							// auto& rigidbody = mesh.AddComponent<RigidbodyComponent>();
							m_SceneHierarchyPanel.SetSelectedEntity(mesh);
						}
					}
					else
						SPIRIT_WARN("Could not load model {0}", modelPath.filename().string());
				}*/
			}
			ImGui::EndDragDropTarget();
		}
		
		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Camera

			// Runtime camera from entity
			// auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			// const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			// const glm::mat4& cameraProjection = camera.GetProjection();
			// glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

			// Editor camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
			float snapValue = 0.5f; // SSnap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform), 
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				tc.Translation = translation;

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Rotation += deltaRotation;

				tc.Scale = scale;
			}
		}



		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();

		ImGui::End();
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;
		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(SPIRIT_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(SPIRIT_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		if (e.GetRepeatCount() > 0 && !(control && e.GetKeyCode() == Key::D))
			return false;

		switch (e.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
				NewScene();

			break;
		}
		case Key::O:
		{
			if (control)
				OpenScene();

			break;
		}
		case Key::S:
		{
			if (control)
			{
				if (shift)
					SaveSceneAs();
				else
					SaveScene();
			}

			break;
		}

		case Key::D:
		{
			if (control)
				DuplicateSelectedEntity();

			break;
		}

		case Key::Delete:
		{
			DeleteSelectedEntity();

			break;
		}

		// Gizmos
		case Key::Q:
			if (!ImGuizmo::IsUsing())
				m_GizmoType = -1;
			break;
		case Key::W:
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case Key::E:
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case Key::R:
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt) && m_ViewportHovered && m_ViewportFocused)
			m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
			Renderer3D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		}
		else
		{
			Renderer2D::BeginScene(m_EditorCamera);
			Renderer3D::BeginScene(m_EditorCamera);
		}

		if (m_ShowPhysicsColliders)
		{
			// Box Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}

			// Circle Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();

				// Calculate z index for translation
				float zIndex = 0.001f;
				glm::vec3 cameraForwardDirection = m_EditorCamera.GetForwardDirection();
				glm::vec3 projectionCollider = cameraForwardDirection * glm::vec3(zIndex);
				for (auto entity : view)
				{
					auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, -projectionCollider.z);
					glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
				}
			}
		}

		Renderer2D::EndScene();
		Renderer3D::EndScene();
	}
	
	void EditorLayer::NewScene()
	{
		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Spirit Scene (*.spirit)\0*.spirit\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();
		
		if (path.extension().string() != ".spirit")
		{
			SPIRIT_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;
			m_ActiveScene = m_EditorScene;
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);

			m_ActiveSceneFilePath = path.string();
		}
	}

	void EditorLayer::SaveScene()
	{
		if (m_ActiveSceneFilePath.empty())
			SaveSceneAs();

		// Duplicated code! (but at least it works for now)
		SceneSerializer serializer(m_ActiveScene);
		serializer.Serialize(m_ActiveSceneFilePath);
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string m_ActiveSceneFilePath = FileDialogs::SaveFile("Spirit Scene (*.spirit)\0*.spirit\0");
		if (!m_ActiveSceneFilePath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(m_ActiveSceneFilePath);
		}
	}

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;

		// Make a copy of the Editor scene
		m_RuntimeScene = Scene::Copy(m_EditorScene);

		m_ActiveScene = m_RuntimeScene;
		m_ActiveScene->OnRuntimeStart();
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;
		
		m_ActiveScene = m_EditorScene;
		m_RuntimeScene = nullptr;
		m_ActiveScene->OnRuntimeStop();
	}

	void EditorLayer::DuplicateSelectedEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
			m_SceneHierarchyPanel.SetSelectedEntity(m_EditorScene->DuplicateEntity(selectedEntity));
	}

	void EditorLayer::DeleteSelectedEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
		{
			m_ActiveScene->DestroyEntity(m_SceneHierarchyPanel.GetSelectionContext());
			m_SceneHierarchyPanel.SetSelectedEntity({});
		}
	}

}