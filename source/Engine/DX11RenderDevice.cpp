#include "DX11RenderDevice.hpp"
#include "WinException.hpp"
#include <fstream>
#include <stb_image.h>
#include "Mesh.hpp"

void DX11RenderDevice::ExtractDXGIresources()
{
	DX_CHECK(pD3d11Device.As(&pDxgiDevice3));
	DX_CHECK(pDxgiDevice3->GetParent(IID_PPV_ARGS(&pDxgiAdapter3))); //GetAdapter or As
	DX_CHECK(pDxgiAdapter3->GetParent(IID_PPV_ARGS(&pDxgiFactory5))); // As
}

void DX11RenderDevice::CreateDeviceAndContext()
{
	D3D_FEATURE_LEVEL m_featureLevel;
	D3D_FEATURE_LEVEL featuresLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	comPtr<ID3D11Device> pDevice;
	comPtr<ID3D11DeviceContext> pContext;
	DX_CHECK(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
		featuresLevels,
		ARRAYSIZE(featuresLevels),
		D3D11_SDK_VERSION,
		&pDevice,
		&m_featureLevel,
		&pContext
	));
	DX_CHECK(pDevice.As(&pD3d11Device));
	DX_CHECK(pContext.As(&pD3d11Context));
}

void DX11RenderDevice::CreateSwapChain(HWND hWnd)
{
	if (pDxgiFactory5 && pD3d11Device)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc {
			.Width = 0,
			.Height = 0,
			.Format = DXGI_FORMAT_B8G8R8A8_UNORM,
			.Stereo = false,
			.SampleDesc {
					.Count = 1,
					.Quality = 0
				},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = 2,
			.Scaling = DXGI_SCALING_NONE,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING,
		};

		comPtr<IDXGISwapChain1> pBaseSwapChain;
		DX_CHECK(pDxgiFactory5->CreateSwapChainForHwnd(
			pD3d11Device.Get(),
			hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&pBaseSwapChain
		));

		DX_CHECK(pBaseSwapChain.As(&pSwapChain3));
	}
}

void DX11RenderDevice::CreateFrameBufferTexture()
{
	//comPtr<ID3D11Texture2D> backBuffer;
	DX11TextureInternal renderColorTexture;
	DX_CHECK(pSwapChain3->GetBuffer(0, IID_PPV_ARGS(&renderColorTexture.pTexture)));
	DX_CHECK(pD3d11Device->CreateRenderTargetView(
		renderColorTexture.pTexture.Get(),
		nullptr, 
		&renderColorTexture.pRTV
	));
	D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };
	renderColorTexture.pTexture->GetDesc(&backBufferDesc);

	D3D11_TEXTURE2D_DESC depthDesc {
		.Width = backBufferDesc.Width,
		.Height = backBufferDesc.Height,
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
		.SampleDesc {
				.Count = 1,
				.Quality = 0
			},
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_DEPTH_STENCIL,
		.CPUAccessFlags = 0,
		.MiscFlags = 0

	};
	DX11TextureInternal depthStencilTexture;
	//comPtr<ID3D11Texture2D> depthStencilTexture;
	DX_CHECK(pD3d11Device->CreateTexture2D(
		&depthDesc, 
		nullptr, 
		&depthStencilTexture.pTexture
	));
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc {
		.Format = depthDesc.Format,
		.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
		.Texture2D { .MipSlice = 0 }
	};

	DX_CHECK(pD3d11Device->CreateDepthStencilView(
		depthStencilTexture.pTexture.Get(), 
		&dsvDesc, 
		&depthStencilTexture.pDSV
	));

	ID3D11RenderTargetView* rtvTargets[] = { renderColorTexture.pRTV.Get()};
	pD3d11Context->OMSetRenderTargets(
		1, 
		rtvTargets, 
		renderColorTexture.pDSV.Get()
	);

	textures.push_back(renderColorTexture);
	TextureHandle colorAttachment = TextureHandle{ static_cast<uint32_t>(textures.size()) };
	frameBuffer.numColorAttachments = textures.size();
	frameBuffer.colorAttachments[0] = colorAttachment;
	textures.push_back(depthStencilTexture);
	TextureHandle depthStencilAttachment = TextureHandle{ static_cast<uint32_t>(textures.size()) };
	frameBuffer.depthStencilAttachments = depthStencilAttachment;

}

void DX11RenderDevice::SetViewport(FLOAT width, FLOAT height)
{
	D3D11_VIEWPORT vp {
		.TopLeftX = 0.0f,
		.TopLeftY = 0.0f,
		.Width = width, 
		.Height = height,
		.MinDepth = 0.0f,
		.MaxDepth = 1.0f
	};

	pD3d11Context->RSSetViewports(1, &vp);
}

std::vector<char> DX11RenderDevice::LoadByteCode(const std::filesystem::path& filename)
{
	std::ifstream file(filename, std::ios::binary || std::ios::ate);
	if (!file.is_open()) return {};
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> buffer(size);
	if (file.read(buffer.data(), size)) return buffer;
	else return {};
}

TextureHandle DX11RenderDevice::LoadTextureFromFile(const std::filesystem::path& filename)
{
	int width = 0; 
	int height = 0; 
	int channels = 0;
	unsigned char* rawPixels = stbi_load(
		filename.string().c_str(), 
		&width, 
		&height, 
		&channels, 
		STBI_rgb_alpha
	);
	if (!rawPixels) return TextureHandle{ 0 };
	TextureHandle handle = CreateTexture(
		static_cast<uint32_t>(width), 
		static_cast<uint32_t>(height), 
		rawPixels
	);
	stbi_image_free(rawPixels);
	return handle;
}

RasterizerStateHandle DX11RenderDevice::CreateRasterizerState(RasterizerParam rParam) noexcept
{
	D3D11_RASTERIZER_DESC rsDesc{
		.FillMode = D3D11_FILL_SOLID,
		.CullMode = (rParam == RasterizerParam::Default ? D3D11_CULL_NONE : D3D11_CULL_FRONT),
		.FrontCounterClockwise = FALSE,
		.DepthBias = (rParam == RasterizerParam::Default ? 0 : 10000),
		.DepthBiasClamp = (rParam == RasterizerParam::Default ? 0.0f : 0.0f),
		.SlopeScaledDepthBias = (rParam == RasterizerParam::Default ? 0.0f : 1.5f),
		.DepthClipEnable = TRUE,
		.ScissorEnable = FALSE,
		.MultisampleEnable = FALSE,
		.AntialiasedLineEnable = FALSE,
	};
	comPtr<ID3D11RasterizerState> pRasterizerState;
	DX_CHECK(pD3d11Device->CreateRasterizerState(&rsDesc, &pRasterizerState));

	rasterizerStates.push_back(pRasterizerState);
	return RasterizerStateHandle{ static_cast<uint32_t>(rasterizerStates.size()) };
}

void DX11RenderDevice::BindRasterizerState(RasterizerStateHandle handle) noexcept
{
	if (handle.id == 0 || handle.id > rasterizerStates.size()) return;
	pD3d11Context->RSSetState(rasterizerStates[handle.id - 1].Get());
}

SamplerStateHandle DX11RenderDevice::CreateSamplerState(SamplerFilter filter, SamplerParam sParam) noexcept
{
	D3D11_TEXTURE_ADDRESS_MODE mode =
		(sParam == SamplerParam::Default ? D3D11_TEXTURE_ADDRESS_CLAMP : D3D11_TEXTURE_ADDRESS_BORDER);
	FLOAT val = sParam == SamplerParam::Default ? 0.0f : 1.0f;
	D3D11_SAMPLER_DESC desc {
		.Filter = (filter == SamplerFilter::Point) ? 
		D3D11_FILTER_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = mode,
		.AddressV = mode,
		.AddressW = mode,
		.ComparisonFunc = D3D11_COMPARISON_NEVER,
		.BorderColor {val,val,val,val},
		.MinLOD = 0,
		.MaxLOD = D3D11_FLOAT32_MAX,
	};
	comPtr<ID3D11SamplerState> sampler;
	DX_CHECK(pD3d11Device->CreateSamplerState(&desc, &sampler));
	samplerStates.push_back(sampler);
	return SamplerStateHandle{ static_cast<uint32_t>(samplerStates.size()) };
}

GeometryBufferHandle DX11RenderDevice::CreateGeometryBuffer(
	const void* v_data, uint32_t v_sizeInBytes, 
	const void* i_data, uint32_t i_sizeInBytes)
{
	VertexBufferHandle vbh = CreateVertexBuffer(v_data, v_sizeInBytes);
	IndexBufferHandle ibh = CreateIndexBuffer(i_data, i_sizeInBytes);
	GeometryBufferHandle gbh {
		.vbh = vbh,
		.ibh = ibh
	};
	return gbh;
}

void DX11RenderDevice::BindSamplerState(uint32_t slot, SamplerStateHandle handle) noexcept
{
	if (handle.id == 0 || handle.id > samplerStates.size()) return;
	pD3d11Context->PSSetSamplers(slot, 1, samplerStates[handle.id - 1].GetAddressOf());
}

VertexBufferHandle DX11RenderDevice::CreateVertexBuffer(const void* data, uint32_t sizeInBytes) noexcept
{
	D3D11_BUFFER_DESC bufferDesc {
		.ByteWidth = sizeInBytes,
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0
	};
	D3D11_SUBRESOURCE_DATA subData { .pSysMem = data };
	comPtr<ID3D11Buffer> buffer;
	DX_CHECK(pD3d11Device->CreateBuffer(&bufferDesc, &subData, &buffer));
	vertexBuffers.push_back(buffer);
	return VertexBufferHandle{ static_cast<uint32_t>(vertexBuffers.size()) };
}

void DX11RenderDevice::BindVertexBuffer(VertexBufferHandle handle, uint32_t stride) noexcept
{
	if (handle.id == 0 || handle.id > vertexBuffers.size()) return;
	ID3D11Buffer* buffer = vertexBuffers[handle.id - 1].Get();
	UINT offset = 0;
	UINT dxStride = stride;
	pD3d11Context->IASetVertexBuffers(0, 1, &buffer, &dxStride, &offset);
	pD3d11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

IndexBufferHandle DX11RenderDevice::CreateIndexBuffer(const void* data, uint32_t sizeInBytes) noexcept
{
	D3D11_BUFFER_DESC bufferDesc {
		.ByteWidth = sizeInBytes,
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0
	};
	D3D11_SUBRESOURCE_DATA subData = { .pSysMem = data };
	comPtr<ID3D11Buffer> buffer;
	DX_CHECK(pD3d11Device->CreateBuffer(&bufferDesc, &subData, &buffer));
	indexBuffers.push_back(buffer);
	return IndexBufferHandle{ static_cast<uint32_t>(indexBuffers.size()) };
}

void DX11RenderDevice::BindIndexBuffer(IndexBufferHandle handle, uint32_t stride) noexcept
{
	if (handle.id == 0 || handle.id > indexBuffers.size()) return;
	ID3D11Buffer* buffer = indexBuffers[handle.id - 1].Get();
	DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32_UINT;
	pD3d11Context->IASetIndexBuffer(buffer, dxgiFormat, 0);
}

ConstantBufferHandle DX11RenderDevice::CreateConstantBuffer(uint32_t sizeInBytes) noexcept
{
	uint32_t alignedSize = (sizeInBytes + 15) & ~15;
	D3D11_BUFFER_DESC bufferDesc {
		.ByteWidth = alignedSize,
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
	};
	comPtr<ID3D11Buffer> buffer;
	DX_CHECK(pD3d11Device->CreateBuffer(&bufferDesc, nullptr, &buffer));
	DX11ConstantBufferInternal iCB {
		.pBuffer = buffer,
		.sizeInBytes = alignedSize
	};
	constantBuffers.push_back(iCB);
	return ConstantBufferHandle{ static_cast<uint32_t>(constantBuffers.size()) };

}

void DX11RenderDevice::UpdateConstantBuffer(ConstantBufferHandle handle, const void* data, uint32_t sizeInBytes) noexcept
{
	if (handle.id == 0 || handle.id > constantBuffers.size()) return;
	const auto& cb = constantBuffers[handle.id - 1];
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DX_CHECK(pD3d11Context->Map(
		cb.pBuffer.Get(),
		0, 
		D3D11_MAP_WRITE_DISCARD, 
		0, 
		&mappedResource)
	);
	memcpy(mappedResource.pData, data, sizeInBytes);
	pD3d11Context->Unmap(cb.pBuffer.Get(), 0);
}

void DX11RenderDevice::BindConstantBuffer(ConstantBufferHandle handle, uint32_t slot, ShaderType stage) noexcept
{
	if (handle.id == 0 || handle.id > constantBuffers.size()) return;
	ID3D11Buffer* buffer = constantBuffers[handle.id - 1].pBuffer.Get();
	if (stage == ShaderType::Vertex) pD3d11Context->VSSetConstantBuffers(slot, 1, &buffer);
	if (stage == ShaderType::Pixel) pD3d11Context->PSSetConstantBuffers(slot, 1, &buffer);
	if (stage == ShaderType::Compute) pD3d11Context->CSSetConstantBuffers(slot, 1, &buffer);

}

TextureHandle DX11RenderDevice::CreateTexture(uint32_t width, uint32_t height, const void* rgbaData) noexcept
{
	if (!rgbaData) return TextureHandle{ 0 };
	D3D11_TEXTURE2D_DESC desc {
		.Width = width,
		.Height = height,
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleDesc {
				.Count = 1,
				.Quality = 0
			},
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_SHADER_RESOURCE,
		.CPUAccessFlags = 0
	};
	D3D11_SUBRESOURCE_DATA subData {
		.pSysMem = rgbaData,
		.SysMemPitch = width * 4
	};

	DX11TextureInternal internalTexture;
	DX_CHECK(pD3d11Device->CreateTexture2D(
		&desc, 
		&subData, 
		&internalTexture.pTexture
	));
	DX_CHECK(pD3d11Device->CreateShaderResourceView(
		internalTexture.pTexture.Get(), 
		nullptr, 
		&internalTexture.pSRV
	));
	textures.push_back(internalTexture);
	return TextureHandle{ static_cast<uint32_t>(textures.size()) };
}

TextureHandle DX11RenderDevice::CreateShadowMapTexture(
	uint32_t width, 
	uint32_t height)
{
	D3D11_TEXTURE2D_DESC desc{
		.Width = width,
		.Height = height,
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_R24G8_TYPELESS,
		.SampleDesc {
				.Count = 1,
				.Quality = 0
			},
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
		.CPUAccessFlags = 0,
		.MiscFlags = 0
	};

	DX11TextureInternal internalTexture;
	DX_CHECK(pD3d11Device->CreateTexture2D(
		&desc,
		nullptr,
		&internalTexture.pTexture
	));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{
		.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
		.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
		.Texture2D { .MipSlice = 0 }
	};

	DX_CHECK(pD3d11Device->CreateDepthStencilView(
		internalTexture.pTexture.Get(), 
		&dsvDesc, 
		&internalTexture.pDSV
	));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{
		.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		.Texture2D {.MipLevels = 1 }
	};

	DX_CHECK(pD3d11Device->CreateShaderResourceView(
		internalTexture.pTexture.Get(),
		&srvDesc,
		&internalTexture.pSRV
	));
	internalTexture.pRTV = nullptr;
	textures.push_back(internalTexture);
	return TextureHandle{ static_cast<uint32_t>(textures.size()) };
}

void DX11RenderDevice::BindTexture(uint32_t slot, TextureHandle handle) noexcept
{
	if (!handle.isValid() || handle.id > textures.size()) return;
	ID3D11ShaderResourceView* srv = textures[handle.id - 1].pSRV.Get();
	pD3d11Context->PSSetShaderResources(slot, 1, &srv);
}

void DX11RenderDevice::UnbindTexture(uint32_t slot) noexcept
{
	ID3D11ShaderResourceView* srv = nullptr;
	pD3d11Context->PSSetShaderResources(slot, 1, &srv);
}

DX11ShaderInternal DX11RenderDevice::CreateShader(const void* byteCode, size_t size, ShaderType stage) noexcept
{
	DX11ShaderInternal shaderInternal{ 0 };
	shaderInternal.stage = stage;
	switch (stage)
	{
		case ShaderType::Vertex:
		{
			ID3D11VertexShader* vs;
			DX_CHECK(pD3d11Device->CreateVertexShader(byteCode, size, nullptr, &vs));
			shaderInternal.pShader = vs;
		}
		break;
		case ShaderType::Pixel: 
		{
			ID3D11PixelShader* ps;
			DX_CHECK(pD3d11Device->CreatePixelShader(byteCode, size, nullptr, &ps));
			shaderInternal.pShader = ps;
		}
		break;
	}
	return shaderInternal;
}

void DX11RenderDevice::BindShader(ShaderHandle handle) noexcept
{
	if (!handle.isValid() && handle.id > shaders.size()) return;
	switch (shaders[handle.id - 1].stage)
	{
		case ShaderType::Vertex:
		{
			ID3D11VertexShader* vs = 
				static_cast<ID3D11VertexShader*>(shaders[handle.id - 1].pShader.Get());
			pD3d11Context->VSSetShader(vs, nullptr, 0);
		}
		break;
		case ShaderType::Pixel:
		{
			ID3D11PixelShader* ps = 
				static_cast<ID3D11PixelShader*>(shaders[handle.id - 1].pShader.Get());
			pD3d11Context->PSSetShader(ps, nullptr, 0);
		}
		break;
		default: 
			return;
	}
}


void DX11RenderDevice::UnbindShader(ShaderType type) noexcept
{
	switch (type)
	{
		case ShaderType::Vertex: pD3d11Context->VSSetShader(nullptr, nullptr, 0); break;
		case ShaderType::Pixel: pD3d11Context->PSSetShader(nullptr, nullptr, 0); break;
		default: return;
	}
}

comPtr<ID3DBlob> DX11RenderDevice::CompileShader(
	const std::filesystem::path& filename, const char* entryPoint, const char* target)
{
	std::filesystem::path absolutePath = std::filesystem::absolute(filename);
	std::ifstream file(absolutePath, std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		return nullptr;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string shaderCode = buffer.str();
	file.close();

	//ОЧИСТКА ОТ BOM (Если в начале файла есть байты EF BB BF)
	if (shaderCode.size() >= 3 &&
		(unsigned char)shaderCode[0] == 0xEF &&
		(unsigned char)shaderCode[1] == 0xBB &&
		(unsigned char)shaderCode[2] == 0xBF)
	{
		shaderCode = shaderCode.substr(3); //Удаляем первые 3 невидимых байта
	}
	comPtr<ID3DBlob> shaderBlob;
	comPtr<ID3DBlob> errorBlob;
	uint32_t compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT hr = D3DCompile(
		shaderCode.c_str(),
		shaderCode.size(),
		filename.string().c_str(),
		nullptr,
		nullptr,
		entryPoint,
		target,
		compileFlags,
		0,
		&shaderBlob,
		&errorBlob
	);

	if (FAILED(hr)) {
		if (errorBlob) {

			std::string str = std::format("syntax error in shader code ({})\n{}", target,
				static_cast<const char*>(errorBlob->GetBufferPointer()));
			OutputDebugStringA(str.c_str());
		}
		else {
			std::string str = std::format("native error D3DCompile: 0x{}", hr);
			OutputDebugStringA(str.c_str());
		}
		return nullptr;
	}
	return shaderBlob;
}

InputLayoutHandle DX11RenderDevice::CreateInputLayout(
	const VertexElement* elements, uint32_t count, 
	const void* vertexShaderByteCode, size_t vsbcSize) noexcept
{
	if (!elements || count == 0 || !vertexShaderByteCode || vsbcSize == 0) return InputLayoutHandle{ 0 };

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements{ count };
	for (uint32_t i = 0; i < count; ++i) {
		inputElements[i].SemanticName = elements[i].semanticName;
		inputElements[i].SemanticIndex = elements[i].semanticIndex;
		inputElements[i].InputSlot = elements[i].slot;
		inputElements[i].AlignedByteOffset = (elements[i].alignedByteOffset == 0xFFFFFFFF) ?
			D3D11_APPEND_ALIGNED_ELEMENT : elements[i].alignedByteOffset;
		inputElements[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElements[i].InstanceDataStepRate = 0;

		switch (elements[i].format)
		{
			case VertexFormat::float2: inputElements[i].Format = DXGI_FORMAT_R32G32_FLOAT; break; 
			case VertexFormat::float3: inputElements[i].Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
			case VertexFormat::float4: inputElements[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
		}
	}

	comPtr<ID3D11InputLayout> pInputLayout;
	DX_CHECK(pD3d11Device->CreateInputLayout(
		inputElements.data(), 
		count, 
		vertexShaderByteCode, 
		vsbcSize, 
		&pInputLayout
	));

	inputLayouts.push_back(pInputLayout);
	return InputLayoutHandle{ static_cast<uint32_t>(inputLayouts.size()) };
}

void DX11RenderDevice::BindInputLayout(InputLayoutHandle handle) noexcept
{
	if (handle.id == 0 || handle.id > inputLayouts.size()) return;
	ID3D11InputLayout* inputLayout = inputLayouts[handle.id - 1].Get();
	pD3d11Context->IASetInputLayout(inputLayout);
}

ShaderHandle DX11RenderDevice::CreateAndCompileShaderFromFile(
	const std::filesystem::path& filename, 
	const char* entryPoint,
	const char* target,
	ShaderType stage)
{
	auto Blob = CompileShader(filename, entryPoint, target);
	DX11ShaderInternal shaderInternal = CreateShader(
		Blob.Get()->GetBufferPointer(),
		Blob.Get()->GetBufferSize(),
		stage
	);
	shaderInternal.pBlob = Blob;
	shaders.push_back(shaderInternal);
	return ShaderHandle{ static_cast<uint32_t>(shaders.size()) };
}

InputLayoutHandle DX11RenderDevice::CreateInputLayoutBySchema(
	std::vector<VertexElement> schema, 
	ShaderHandle vsHandle)
{
	if (!vsHandle.isValid() && vsHandle.id > shaders.size()) return InputLayoutHandle{ 0 };
	ID3DBlob* blob = shaders[vsHandle.id - 1].pBlob.Get();
	InputLayoutHandle layoutHandle = CreateInputLayout(
		schema.data(), 
		3,
		blob->GetBufferPointer(),
		blob->GetBufferSize()
	);
	return layoutHandle;
}

void DX11RenderDevice::Draw(uint32_t vertexCount) noexcept
{
	pD3d11Context->Draw(vertexCount, 0);
}

void DX11RenderDevice::DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) noexcept
{
	pD3d11Context->DrawIndexed(indexCount, startIndex, baseVertex);
}

void DX11RenderDevice::Initialize(float width, float height)
{
	CreateDeviceAndContext();
	ExtractDXGIresources();
	CreateSwapChain(hwnd);
	CreateFrameBufferTexture();
	SetViewport(width, height);
}

void DX11RenderDevice::ClearColor(TextureHandle handle)
{
	if (!handle.isValid()) return;
	const FLOAT color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	pD3d11Context->ClearRenderTargetView(
		textures[handle.id - 1].pRTV.Get(),
		color
	);
}

void DX11RenderDevice::ClearDepth(TextureHandle handle)
{
	pD3d11Context->ClearDepthStencilView(
		textures[handle.id - 1].pDSV.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);
}

void DX11RenderDevice::SetFrameBuffer(const FrameBuffer& fb)
{
	ID3D11RenderTargetView* pRTV = fb.colorAttachments[0].isValid() ?
		textures[fb.colorAttachments[0].id - 1].pRTV.Get() : nullptr;
	ID3D11RenderTargetView* rtvTargets[] = { pRTV };
	ID3D11DepthStencilView* pDSV = fb.depthStencilAttachments.isValid() ?
		textures[fb.depthStencilAttachments.id - 1].pDSV.Get() : nullptr;
	pD3d11Context->OMSetRenderTargets(1, rtvTargets, pDSV);
}

void DX11RenderDevice::SetRenderTarget(TextureHandle colorTex, TextureHandle depthTex)
{
	FrameBuffer fb = { 0 };
	fb.colorAttachments[0] = colorTex;
	fb.depthStencilAttachments = depthTex;
	fb.numColorAttachments = 1;
	SetFrameBuffer(fb);
}

void DX11RenderDevice::BeginFrame() noexcept
{
	ClearColor(frameBuffer.colorAttachments[0]);
	ClearDepth(frameBuffer.depthStencilAttachments);
	SetFrameBuffer(frameBuffer);
}

void DX11RenderDevice::EndFrame(bool vsync) noexcept
{
	pD3d11Context->DiscardView1(
		textures[frameBuffer.depthStencilAttachments.id - 1].pDSV.Get(),
		nullptr, 
		0
	);
	UINT presentFlags = 0;
	UINT syncInterval = vsync ? 1 : 0;
	if (!vsync) presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
	DX_CHECK(pSwapChain3->Present(syncInterval, presentFlags));
}
