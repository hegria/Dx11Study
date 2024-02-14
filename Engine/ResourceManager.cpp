#include "pch.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Viewport.h"
#include <filesystem>

void ResourceManager::Init()
{
	CreateDefaultMesh();
	CreateDefaultShader();
	CreateDefaultMaterial();
}

std::shared_ptr<Texture> ResourceManager::GetOrAddTexture(const wstring& key, const wstring& path)
{
	shared_ptr<Texture> texture = Get<Texture>(key);

	if (filesystem::exists(filesystem::path(path)) == false)
		return nullptr;

	texture = Load<Texture>(key, path);

	if (texture == nullptr)
	{
		texture = make_shared<Texture>();
		texture->Load(path);
		Add(key, texture);
	}

	return texture;
}

void ResourceManager::CreateDefaultMesh()
{
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateQuad();
		Add(L"Rectangle", mesh);
	}
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateCube();
		Add(L"Cube", mesh);
	}
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateSphere();
		Add(L"Sphere", mesh);
	}
}

void ResourceManager::CreateDefaultShader()
{
	{
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\skybox.fx");
		Add<Shader>(L"Skybox", shader);
	}


	// Deferred (Deferred)
	{

		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\deferred.fx");
		Add<Shader>(L"Deferred", shader);
	}

	// Forward (Forward)
	{
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\forward.fx");
		Add<Shader>(L"Forward", shader);
	}

	// Texture (Forward)
	{
		
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\forward.fx");
		Add<Shader>(L"Texture", shader);
	}

	// DirLight
	{
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\lighting.fx");
		Add<Shader>(L"DirLight", shader);
	}

	// PointLight
	{
		
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\lighting.fx");
		Add<Shader>(L"PointLight", shader);
	}

	// Final
	{
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\lighting.fx");
		Add<Shader>(L"Final", shader);
	}

	// Compute Shader
	{
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\compute.fx");
		Add<Shader>(L"ComputeShader", shader);
	}

	// Particle
	{
		
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\particle.fx");
		Add<Shader>(L"Particle", shader);
	}

	// ComputeParticle
	{
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\particle.fx");
		Add<Shader>(L"ComputeParticle", shader);
	}

	// Shadow
	{
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\shadow.fx");
		Add<Shader>(L"Shadow", shader);
	}

	// Tessellation
	{
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shader\\tessellation.fx");
		Add<Shader>(L"Tessellation", shader);
	}

	// Terrain
	{
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\terrain.fx");
		Add<Shader>(L"Terrain", shader);
	}

	// ComputeAnimation
	{
		shared_ptr<Shader> shader = make_shared<Shader>(L"..\\Shaders\\animation.fx");
		Add<Shader>(L"ComputeAnimation", shader);
	}
}

void ResourceManager::CreateDefaultMaterial()
{
	// Skybox
	{
		shared_ptr<Shader> shader = Get<Shader>(L"Skybox");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Skybox", material);
	}

	// DirLight
	{
		shared_ptr<Shader> shader = Get<Shader>(L"DirLight");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetDiffuseMap(Get<Texture>(L"PositionTarget"));
		material->SetSpecularMap(Get<Texture>(L"NormalTarget"));
		Add<Material>(L"DirLight", material);
	}

	// PointLight
	{
		const Viewport& window = GRAPHICS->GetViewport();
		float resolutionArray[4] = { window.GetWidth(), window.GetHeight(), 0.0f, 0.0f };
		shared_ptr<Shader> shader = Get<Shader>(L"PointLight");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetDiffuseMap(Get<Texture>(L"PositionTarget"));
		material->SetSpecularMap(Get<Texture>(L"NormalTarget"));
		shader->GetVector("Resolution")->SetFloatVector(resolutionArray);
		Add<Material>(L"PointLight", material);
	}

	// Final
	{
		shared_ptr<Shader> shader = Get<Shader>(L"Final");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetDiffuseMap(Get<Texture>(L"DiffuseTarget"));
		material->SetSpecularMap(Get<Texture>(L"DiffuseLightTarget"));
		material->SetNormalMap(Get<Texture>(L"SpecularLightTarget"));
		Add<Material>(L"Final", material);
	}

	// Compute Shader
	{
		shared_ptr<Shader> shader = Get<Shader>(L"ComputeShader");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"ComputeShader", material);
	}

	// Particle
	{
		shared_ptr<Shader> shader = Get<Shader>(L"Particle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Particle", material);
	}

	// ComputeParticle
	{
		shared_ptr<Shader> shader = Get<Shader>(L"ComputeParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);

		Add<Material>(L"ComputeParticle", material);
	}

	// GameObject
	{
		shared_ptr<Shader> shader = Get<Shader>(L"Deferred");
		shared_ptr<Texture> texture = Load<Texture>(L"Leather", L"..\\Resources\\Textures\\Leather.jpg");
		shared_ptr<Texture> texture2 = Load<Texture>(L"Leather_Normal", L"..\\Resources\\Textures\\Leather_Normal.jpg");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetDiffuseMap(texture);
		material->SetNormalMap(texture2);
		Add<Material>(L"GameObject", material);
	}

	// Shadow
	{
		shared_ptr<Shader> shader = Get<Shader>(L"Shadow");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Shadow", material);
	}

	// Tessellation
	{
		shared_ptr<Shader> shader = Get<Shader>(L"Tessellation");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Tessellation", material);
	}

	// Terrain
	{
		shared_ptr<Shader> shader = Get<Shader>(L"Terrain");
		shared_ptr<Texture> texture = Load<Texture>(L"Terrain", L"..\\Resources\\Textures\\Terrain\\terrain.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetDiffuseMap(texture);
		Add<Material>(L"Terrain", material);
	}

	// ComputeAnimation
	{
		shared_ptr<Shader> shader = Get<Shader>(L"ComputeAnimation");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);

		Add<Material>(L"ComputeAnimation", material);
	}
}
