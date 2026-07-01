#include "RenderPipeline.hpp"
#include "DX11RenderDevice.hpp"
#include "Object3D.hpp"
#include<memory>

RenderPipeline::RenderPipeline(HWND hwnd)
{
	dxhal = std::make_unique<DX11RenderDevice>(hwnd);
}

void RenderPipeline::Initialize(float width, float height)
{
	dxhal->Initialize(width, height);

	cam.SetProjection(ProjectionType::Perspective, width, height);
	cam.SetPosition(0.0f, 20.0f, -50.0f);
}

void RenderPipeline::CreateResources(Mesh& mesh)
{
	//th = wnd.gfx().LoadTextureFromFile(L"assets/textures/texAtlas.png");
	//if (!wrapper.Initialize(wnd.getHandle(), wnd.gfx())) throw std::exception("imgui init error");
	RasterizerStateHandle rasterizerHandle = dxhal->CreateRasterizerState(RasterizerParam::Default);
	RasterizerStateHandle shadowRasterizerHandle = dxhal->CreateRasterizerState(RasterizerParam::ShadowMap);
	SamplerStateHandle samplerHandle = dxhal->CreateSamplerState(SamplerFilter::Point, SamplerParam::Default);
	SamplerStateHandle shadowSamplerHandle = dxhal->CreateSamplerState(SamplerFilter::Point, SamplerParam::ShadowMap);

	std::vector<VertexElement> schema {
		{"POSITION", 0, VertexFormat::float3, 0, 0},
		{"COLOR", 0, VertexFormat::float4, 0, 12}
	};
	ShaderHandle shadowVSHandle = dxhal->CreateAndCompileShaderFromFile(
		L"assets/shaders/ShadowVertexShader.hlsl",
		"main",
		"vs_5_0",
		ShaderType::Vertex
	);
	ShaderHandle vsHandle = dxhal->CreateAndCompileShaderFromFile(
		L"assets/shaders/VertexShader.hlsl",
		"main",
		"vs_5_0",
		ShaderType::Vertex
	);
	ShaderHandle psHandle = dxhal->CreateAndCompileShaderFromFile(
		L"assets/shaders/PixelShader.hlsl",
		"main",
		"ps_5_0",
		ShaderType::Pixel
	);
	InputLayoutHandle layoutHandle = dxhal->CreateInputLayoutBySchema(schema, vsHandle);
	InputLayoutHandle shadowlayoutHandle = dxhal->CreateInputLayoutBySchema(schema, shadowVSHandle);
	GeometryBufferHandle gBuffer = dxhal->CreateGeometryBuffer(
		mesh.VertexData(), mesh.VertexSize(),
		mesh.IndexData(), mesh.IndexSize());
	ConstantBufferHandle cameraCBuffer = dxhal->CreateConstantBuffer(size_xmmatrix);
	ConstantBufferHandle objectCBuffer = dxhal->CreateConstantBuffer(size_xmmatrix);
	ConstantBufferHandle lightCBuffer = dxhal->CreateConstantBuffer(size_xmmatrix);

	TextureHandle shadowMapHandle = dxhal->CreateShadowMapTexture(
		static_cast<uint32_t>(1024), 
		static_cast<uint32_t>(1024));
	rh = {
		.rsh = rasterizerHandle,
		.ssh = samplerHandle,
		.gbh = gBuffer,
		.camerabh = cameraCBuffer,
		.objectbh = objectCBuffer,
		.ilh = layoutHandle,
		.vsh = vsHandle,
		.psh = psHandle,
		.shadowvsh = shadowVSHandle,
		.shadowilh = shadowlayoutHandle,
		.lightcbh = lightCBuffer,
		.shadowmapth = shadowMapHandle,
		.shadowrsh = shadowRasterizerHandle,
		.shadowssh = shadowSamplerHandle
	};
}

void RenderPipeline::Render(Object3D& object)
{
	DirectX::XMMATRIX cameraMatrix = cam.GetViewProjectionMatrix();
	dxhal->UpdateConstantBuffer(rh.camerabh, &cameraMatrix, size_xmmatrix);
	DirectX::XMMATRIX lightMatrix = dirLight.GetLightSpaceMatrix();
	dxhal->UpdateConstantBuffer(rh.lightcbh, &lightMatrix, size_xmmatrix);

	//======================SHADOW PASS====================
	dxhal->UnbindTexture(1);
	dxhal->SetRenderTarget(TextureHandle{ 0 }, rh.shadowmapth);
	dxhal->SetViewport(1024, 1024);
	dxhal->ClearDepth(rh.shadowmapth);

	dxhal->BindRasterizerState(rh.shadowrsh);

	dxhal->UnbindShader(ShaderType::Pixel);
	dxhal->BindShader(rh.shadowvsh);
	dxhal->BindInputLayout(rh.shadowilh);

	DirectX::XMMATRIX objectMatrix = object.GetMatrix();
	dxhal->UpdateConstantBuffer(rh.objectbh, &objectMatrix, size_xmmatrix);
	dxhal->BindConstantBuffer(rh.objectbh, 1, ShaderType::Vertex);
	dxhal->BindConstantBuffer(rh.lightcbh, 2, ShaderType::Vertex);

	dxhal->BindVertexBuffer(rh.gbh.vbh, size_vertex3d);
	dxhal->BindIndexBuffer(rh.gbh.ibh, size_vertex3d);
	dxhal->DrawIndexed(object.GetMesh().IndexCount(), 0, 0);

	//======================MAIN PASS======================
	FrameBuffer frameBuffer = dxhal->GetFrameBuffer();
	dxhal->SetRenderTarget(frameBuffer.colorAttachments[0], frameBuffer.depthStencilAttachments);
	dxhal->SetViewport(1000, 600);
	dxhal->ClearColor(frameBuffer.colorAttachments[0]);
	dxhal->ClearDepth(frameBuffer.depthStencilAttachments);

	dxhal->BindRasterizerState(rh.rsh);
	dxhal->BindShader(rh.vsh);
	dxhal->BindShader(rh.psh);
	dxhal->BindInputLayout(rh.ilh);

	dxhal->BindConstantBuffer(rh.camerabh, 0, ShaderType::Vertex);
	dxhal->BindConstantBuffer(rh.objectbh, 1, ShaderType::Vertex);
	dxhal->BindConstantBuffer(rh.lightcbh, 2, ShaderType::Vertex);

	dxhal->BindTexture(1, rh.shadowmapth);
	dxhal->BindSamplerState(1, rh.shadowssh);

	dxhal->BindVertexBuffer(rh.gbh.vbh, size_vertex3d);
	dxhal->BindIndexBuffer(rh.gbh.ibh, size_vertex3d);
	dxhal->DrawIndexed(object.GetMesh().IndexCount(), 0, 0);
	dxhal->EndFrame(true);
}
