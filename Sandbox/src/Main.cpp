#include <Grapple/Core/Application.h>
#include <Grapple/Renderer/RenderCommand.h>
#include <Grapple/Core/EntryPoint.h>

#include <Grapple/Renderer/Shader.h>

#include <glm/glm.hpp>

using namespace Grapple;

class SandboxApplication : public Application
{
public:
	SandboxApplication()
	{
		m_Shader = Shader::Create("Shader.glsl");

		glm::vec3 vertices[] =
		{
			{ -0.5f, -0.5f, 0.0f },
			{ -0.5f,  0.5f, 0.0f },
			{  0.5f,  0.5f, 0.0f },
			{  0.5f, -0.5f, 0.0f },
		};

		uint32_t indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		m_Quad = VertexArray::Create();
		m_Vertices = VertexBuffer::Create(sizeof(vertices), vertices);
		m_Indices = IndexBuffer::Create(6, indices);

		m_Vertices->SetLayout({
			BufferLayoutElement("Vertex", ShaderDataType::Float3),
		});

		m_Quad->SetIndexBuffer(m_Indices);
		m_Quad->AddVertexBuffer(m_Vertices);

		RenderCommand::SetClearColor(0.1f, 0.2f, 0.3f, 1);
	}
public:
	virtual void OnUpdate() override
	{
		RenderCommand::Clear();

		m_Shader->Bind();
		RenderCommand::DrawIndex(m_Quad);
	}
private:
	Ref<VertexArray> m_Quad;
	Ref<VertexBuffer> m_Vertices;
	Ref<IndexBuffer> m_Indices;

	Ref<Shader> m_Shader;
};

Scope<Application> Grapple::CreateGrappleApplication(Grapple::CommandLineArguments arguments)
{
	return CreateScope<SandboxApplication>();
}