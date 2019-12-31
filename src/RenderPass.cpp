#include "RenderPass.hpp"

using namespace ofxDeferred;

const std::string RenderPassRegistry::Bg("BgPass");
const std::string RenderPassRegistry::Bloom("BloomPass");
const std::string RenderPassRegistry::Blur("BlurPass");
const std::string RenderPassRegistry::Dof("DofPass");
const std::string RenderPassRegistry::Edge("EdgePass");
const std::string RenderPassRegistry::Fog("FogPass");
const std::string RenderPassRegistry::PointLight("PointLightPass");
const std::string RenderPassRegistry::ShadowLight("ShadowLightPass");
const std::string RenderPassRegistry::Ssao("SsaoPass");

RenderPass::RenderPass(const glm::vec2& sz, const std::string& n) : size(sz), name(n) {
	group.setName(name);
	group.add(enabled.set("active", true));
}
