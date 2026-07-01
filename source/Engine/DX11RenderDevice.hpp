#pragma once

#include<d3d11_4.h>
#include<wrl/client.h>
#include<dxgi.h>
#include<dxgi1_2.h>
#include<dxgi1_4.h>
#include<dxgi1_6.h>
#include<vector>
#include<filesystem>
#include<d3dcompiler.h>
#include<DirectXMath.h>

#include "IRenderDevice.hpp"
#include "DX11RenderTypes.hpp"
#include "Mesh.hpp"

template<typename T> using comPtr = Microsoft::WRL::ComPtr<T>;

struct DX11ConstantBufferInternal {
	comPtr<ID3D11Buffer> pBuffer;
	uint32_t sizeInBytes;
};

struct DX11TextureInternal {
	comPtr<ID3D11Texture2D> pTexture;
	comPtr<ID3D11ShaderResourceView> pSRV;
	comPtr<ID3D11RenderTargetView> pRTV;
	comPtr<ID3D11DepthStencilView> pDSV;
	//comPtr<ID3D11UnorderedAccessView> pUAV;
};

struct FrameBuffer {
	TextureHandle colorAttachments[8] = { 0 };
	TextureHandle depthStencilAttachments;
	uint32_t numColorAttachments;
};

struct DX11ShaderInternal {
	comPtr<ID3DBlob> pBlob;
	comPtr<ID3D11DeviceChild> pShader;
	ShaderType stage;
};

class DX11RenderDevice : public IRenderDevice {
public:
	DX11RenderDevice(HWND hwnd)
		: hwnd{ hwnd }
	{}
	DX11RenderDevice(const DX11RenderDevice&) = delete;
	DX11RenderDevice& operator=(const DX11RenderDevice&) = delete;
	~DX11RenderDevice() = default;
public:
	void Initialize(float width, float height) override;
	void BeginFrame() noexcept override;
	void EndFrame(bool vsync) noexcept override;
	void Draw(uint32_t vertexCount) noexcept override;
	void DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) noexcept override;
	void ClearColor(TextureHandle handle);
	void ClearDepth(TextureHandle handle);
	void SetRenderTarget(TextureHandle rtvTex, TextureHandle depthTex);
	void SetViewport(FLOAT width, FLOAT height);
	const FrameBuffer GetFrameBuffer() const { return frameBuffer; }
	TextureHandle LoadTextureFromFile(const std::filesystem::path& filename);
	void BindTexture(uint32_t slot, TextureHandle handle) noexcept;
	void UnbindTexture(uint32_t slot) noexcept;
	void BindSamplerState(uint32_t slot, SamplerStateHandle handle) noexcept;
	void UpdateConstantBuffer(ConstantBufferHandle handle, const void* data, uint32_t sizeInBytes) noexcept;
	void BindConstantBuffer(ConstantBufferHandle handle, uint32_t slot, ShaderType stage) noexcept;
private:
	void ExtractDXGIresources();
	void CreateDeviceAndContext();
	void CreateSwapChain(HWND hWnd);
	void CreateFrameBufferTexture();
	void SetFrameBuffer(const FrameBuffer& fb);
private:
	std::vector<char> LoadByteCode(const std::filesystem::path& filename);
	std::vector<Vertex3D> GenerateVertices(uint32_t width, uint32_t depth);
	std::vector<uint32_t> GenerateIndices(uint32_t width, uint32_t depth);
public:
	RasterizerStateHandle CreateRasterizerState(RasterizerParam rParam) noexcept;
	void BindRasterizerState(RasterizerStateHandle handle) noexcept;
	SamplerStateHandle CreateSamplerState(SamplerFilter filter, SamplerParam sParam) noexcept;
	GeometryBufferHandle CreateGeometryBuffer(
		const void* v_data,
		uint32_t v_sizeInBytes,
		const void* i_data,
		uint32_t i_sizeInBytes
	);
	VertexBufferHandle CreateVertexBuffer(const void* data, uint32_t sizeInBytes) noexcept;
	void BindVertexBuffer(VertexBufferHandle handle, uint32_t stride) noexcept;
	IndexBufferHandle CreateIndexBuffer(const void* data, uint32_t sizeInBytes) noexcept;
	void BindIndexBuffer(IndexBufferHandle handle, uint32_t stride) noexcept;
	ConstantBufferHandle CreateConstantBuffer(uint32_t sizeInBytes) noexcept;

	TextureHandle CreateTexture(uint32_t width, uint32_t height, const void* rgbaData) noexcept;
	TextureHandle CreateShadowMapTexture(uint32_t width, uint32_t height);


	DX11ShaderInternal CreateShader(
		const void* byteCode, 
		size_t size, 
		ShaderType stage) noexcept;
	void BindShader(ShaderHandle handle) noexcept;
	void UnbindShader(ShaderType type) noexcept;
	comPtr<ID3DBlob> CompileShader(
		const std::filesystem::path& filename, 
		const char* entryPoint, 
		const char* target);
	ShaderHandle CreateAndCompileShaderFromFile(
		const std::filesystem::path& filename,
		const char* entryPoint,
		const char* target,
		ShaderType stage);

	InputLayoutHandle CreateInputLayout(
		const VertexElement* elements, 
		uint32_t count, 
		const void* vsByteCode, 
		size_t vsbcSize) noexcept;
	void BindInputLayout(InputLayoutHandle handle) noexcept;
	InputLayoutHandle CreateInputLayoutBySchema(std::vector<VertexElement> schema, ShaderHandle vsHandle);
private:
	//======================RESOURCES OF HARDWARE ABSTRACTION LAYER======================
	//1.0 SYSTEM
	comPtr<ID3D11Device1> pD3d11Device = nullptr;
	comPtr<ID3D11DeviceContext1> pD3d11Context = nullptr;
	comPtr<IDXGISwapChain3> pSwapChain3 = nullptr;
	comPtr<IDXGIDevice3> pDxgiDevice3 = nullptr;
	comPtr<IDXGIAdapter3> pDxgiAdapter3 = nullptr;
	comPtr<IDXGIFactory5> pDxgiFactory5 = nullptr;

	//2.0 GEOMETRY BUFFERS
	std::vector<comPtr<ID3D11Buffer>> vertexBuffers;
	std::vector<comPtr<ID3D11Buffer>> indexBuffers;
	std::vector<DX11ConstantBufferInternal> constantBuffers;

	//3.0 TEXTURES (ID3D11Texture1D, ID3D11Texture2D, ID3D11Texture3D) 
	std::vector<DX11TextureInternal> textures;

	//4.0 VIEWS (ID3D11ShaderResourceView, ID3D11RenderTargetView, 
	// ID3D11DepthStencilView, ID3D11UnorderedAccessView)
	FrameBuffer frameBuffer;

	//5.0 STATES (ID3D11BlendState, ID3D11DepthStencilState)
	std::vector<comPtr<ID3D11RasterizerState>> rasterizerStates;
	std::vector<comPtr<ID3D11SamplerState>> samplerStates;

	//6.0 SHADERS (ID3D11GeometryShader, ID3D11HullShader, ID3D11DomainShader)
	std::vector<DX11ShaderInternal> shaders;

	//7.0 INPUTLAYOUTS
	std::vector<comPtr<ID3D11InputLayout>> inputLayouts;
private:
	HWND hwnd;
};