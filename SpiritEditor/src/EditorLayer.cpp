#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <SpiritEngine/Scene/Scene.h>

#include <SpiritEngine/Audio/SpiritAudio.h>

//#include "ConsoleSpirit3D/ConsoleSpirit3D.cpp"

//#define SPIRIT_WINDOW_HIERARCHY ImGui::Begin("Hierarchy"); ImGui::End();

namespace SpiritEngine {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
	{
	}

	void EditorLayer::OnAttach()
	{
		SPIRIT_PROFILE_FUNCTION();

		m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		//m_CheckerboardTexture = Texture2D::Create("assets/textures/ChernoLogo.png");

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

		// Entity
		auto square = m_ActiveScene->CreateEntity("Green Square");
		square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

		m_SquareEntity = square;

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondCamera = m_ActiveScene->CreateEntity("Clip-Space Entity");
		auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
		cc.Primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate()
			{
				auto& transform = GetComponent<TransformComponent>().Transform;
				transform[3][0] = rand() % 10 - 5.0f;
			}

			void OnDestroy()
			{
			}

			void OnUpdate(Timestep ts)
			{
				auto& transform = GetComponent<TransformComponent>().Transform;
				float speed = 5.0f;

				if (Input::IsKeyPressed(KeyCode::A))
					transform[3][0] -= speed * ts;
				if (Input::IsKeyPressed(KeyCode::D))
					transform[3][0] += speed * ts;
				if (Input::IsKeyPressed(KeyCode::W))
					transform[3][1] += speed * ts;
				if (Input::IsKeyPressed(KeyCode::S))
					transform[3][1] -= speed * ts;
			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Update
		if (m_ViewportFocused)
			m_CameraController.OnUpdate(ts);

		// Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		// Update scene
		m_ActiveScene->OnUpdate(ts);

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
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				/*if (is3D)
				{
					if (ImGui::MenuItem("Make 2D")) Make2D();
				}
				else
				{
					if (ImGui::MenuItem("Make 3D")) Make3D();
				}*/
				ImGui::EndMenu();
			}

			/*if (ImGui::BeginMenu("Audio"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Play Music")) PlayMusic();
				if (ImGui::MenuItem("Rename To Test From SpiritAudio")) RenameAudio("assets/audio/BackgroundMusic", ".spiritaudio", "assets/audio/BackgroundMusic", ".test");
				if (ImGui::MenuItem("Rename To MP3 From SpiritAudio")) RenameAudio("assets/audio/BackgroundMusic", ".spiritaudio", "assets/audio/BackgroundMusic", ".mp3");
				if (ImGui::MenuItem("Rename To WAV From SpiritAudio")) RenameAudio("assets/audio/BackgroundMusic", ".spiritaudio", "assets/audio/BackgroundMusic", ".wav");
				if (ImGui::MenuItem("Rename To OGG From SpiritAudio")) RenameAudio("assets/audio/BackgroundMusic", ".spiritaudio", "assets/audio/BackgroundMusic", ".ogg");
				if (ImGui::MenuItem("Rename To MIDI From SpiritAudio")) RenameAudio("assets/audio/BackgroundMusic", ".spiritaudio", "assets/audio/BackgroundMusic", ".midi");
				if (ImGui::MenuItem("Rename To AIFF From SpiritAudio")) RenameAudio("assets/audio/BackgroundMusic", ".spiritaudio", "assets/audio/BackgroundMusic", ".aiff");
				if (ImGui::MenuItem("Rename To CDA From SpiritAudio")) RenameAudio("assets/audio/BackgroundMusic", ".spiritaudio", "assets/audio/BackgroundMusic", ".cda");
				if (ImGui::MenuItem("Rename To SpiritAudio From Test")) RenameAudio("assets/audio/BackgroundMusic", ".test", "assets/audio/BackgroundMusic", ".spiritaudio");
				if (ImGui::MenuItem("Rename To SpiritAudio From MP3")) RenameAudio("assets/audio/BackgroundMusic", ".mp3", "assets/audio/BackgroundMusic", ".spiritaudio");
				if (ImGui::MenuItem("Rename To SpiritAudio From WAV")) RenameAudio("assets/audio/BackgroundMusic", ".wav", "assets/audio/BackgroundMusic", ".spiritaudio");
				if (ImGui::MenuItem("Rename To SpiritAudio From OGG")) RenameAudio("assets/audio/BackgroundMusic", ".ogg", "assets/audio/BackgroundMusic", ".spiritaudio");
				if (ImGui::MenuItem("Rename To SpiritAudio From MIDI")) RenameAudio("assets/audio/BackgroundMusic", ".midi", "assets/audio/BackgroundMusic", ".spiritaudio");
				if (ImGui::MenuItem("Rename To SpiritAudio From AIFF")) RenameAudio("assets/audio/BackgroundMusic", ".aiff", "assets/audio/BackgroundMusic", ".spiritaudio");
				if (ImGui::MenuItem("Rename To SpiritAudio From CDA")) RenameAudio("assets/audio/BackgroundMusic", ".cda", "assets/audio/BackgroundMusic", ".spiritaudio");
				ImGui::EndMenu();
			}*/

			/*if (ImGui::BeginMenu("Window"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Hierarchy")) SPIRIT_WINDOW_HIERARCHY;
				ImGui::EndMenu();
			}*/

			if (ImGui::BeginMenu("Edit"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				//if (ImGui::MenuItem("Exit")) Application::Get().Close();
				if (is3D)
				{
					if (ImGui::MenuItem("Make 2D")) Make2D();
					//if (ImGui::MenuItem("Open Console Spirit3D"));
				}
				else
				{
					if (ImGui::MenuItem("Make 3D")) Make3D();
					//if (ImGui::MenuItem("Open Console Spirit2D")) MakeConsole();
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::Begin("Inspector");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		if (m_SquareEntity)
		{
			ImGui::Separator();
			auto& tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
			ImGui::Text("%s", tag.c_str());

			auto& squareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
			ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));
			ImGui::Separator();
		}

		ImGui::DragFloat3("Camera Transform",
			glm::value_ptr(m_CameraEntity.GetComponent<TransformComponent>().Transform[3]));

		if (ImGui::Checkbox("Camera A", &m_PrimaryCamera))
		{
			m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
			m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
		}

		{
			auto& camera = m_SecondCamera.GetComponent<CameraComponent>().Camera;
			float orthoSize = camera.GetOrthographicSize();
			if (ImGui::DragFloat("Second Camera Ortho Size", &orthoSize))
				camera.SetOrthographicSize(orthoSize);
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();



		//ImGui::Begin("Hierarchy");

		/*auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));*/

		//ImGui::End();

		//SPIRIT_WINDOW_HIERARCHY
		/*ImGui::Begin("Hierarchy");
		ImGui::End();*/
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
	}

	void EditorLayer::Make2D()
	{
		is3D = false;
	}

	void EditorLayer::Make3D()
	{
		is3D = true;
	}

	void EditorLayer::MakeConsole()
	{
		if (!isConsole)
		{
			isConsole = true;

			if (is3D)
			{
				//ConsoleSpirit3D consoleSpirit3D;
				//hConsoleSpirit3D ConsoleSpirit3D;
				//consoleSpirit3D.mainFunction();
			}
			else
			{
			}
		}
	}

	void EditorLayer::PlayMusic()
	{
		// Initialize the audio engine
		Audio::Init();
		// Load audio source from file
		auto source = AudioSource::LoadFromFile("assets/audio/BackgroundMusic.spiritaudio", false);

		// Make it loop forever
		source.SetLoop(true);

		// Play audio source
		Audio::Play(source);

		/*for (int i = 0; i < 7; i++)
		{
			if (i == 0)
			{
				auto source = AudioSource::LoadFromFile("assets/audio/BackgroundMusic.spiritaudio", false);
			}
			else if (i == 1)
			{
				auto source = AudioSource::LoadFromFile("assets/audio/BackgroundMusic.mp3", false);
			}
			else if (i == 2)
			{
				auto source = AudioSource::LoadFromFile("assets/audio/BackgroundMusic.wav", false);
			}
			else if (i == 3)
			{
				auto source = AudioSource::LoadFromFile("assets/audio/BackgroundMusic.ogg", false);
			}
			else if (i == 4)
			{
				auto source = AudioSource::LoadFromFile("assets/audio/BackgroundMusic.midi", false);
			}
			else if (i == 5)
			{
				auto source = AudioSource::LoadFromFile("assets/audio/BackgroundMusic.aiff", false);
			}
			else if (i == 6)
			{
				auto source = AudioSource::LoadFromFile("assets/audio/BackgroundMusic.cda", false);
			}
			// Make it loop forever
			source.SetLoop(true);

			// Play audio source
			Audio::Play(source);
		}*/
	}

	void EditorLayer::RenameAudio(std::string oldName, std::string oldFormat, std::string newName, std::string newFormat)
	{
		auto oldFile = oldName + oldFormat;
		auto newFile = newName + newFormat;
		rename(oldFile.c_str(), newFile.c_str());
	}

}