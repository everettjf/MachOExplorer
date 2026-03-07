#ifndef MOEX_DYLDSHAREDCACHEVIEWNODE_H
#define MOEX_DYLDSHAREDCACHEVIEWNODE_H

#include "../ViewNode.h"
#include "libmoex/node/DyldSharedCache.h"

MOEX_NAMESPACE_BEGIN

class DyldSharedCacheMappingsViewNode : public ViewNode {
private:
    DyldSharedCachePtr cache_;
public:
    void Init(DyldSharedCachePtr cache) { cache_ = cache; }
    std::string GetDisplayName() override { return "Mappings"; }
    void InitViewDatas() override;
};
using DyldSharedCacheMappingsViewNodePtr = std::shared_ptr<DyldSharedCacheMappingsViewNode>;

class DyldSharedCacheImagesViewNode : public ViewNode {
private:
    DyldSharedCachePtr cache_;
public:
    void Init(DyldSharedCachePtr cache) { cache_ = cache; }
    std::string GetDisplayName() override { return "Images"; }
    void InitViewDatas() override;
};
using DyldSharedCacheImagesViewNodePtr = std::shared_ptr<DyldSharedCacheImagesViewNode>;

class DyldSharedCacheViewNode : public ViewNode {
private:
    DyldSharedCachePtr cache_;
    DyldSharedCacheMappingsViewNodePtr mappings_;
    DyldSharedCacheImagesViewNodePtr images_;
public:
    void Init(DyldSharedCachePtr cache);
    std::string GetDisplayName() override { return "Dyld Shared Cache"; }
    void InitViewDatas() override;
    void ForEachChild(std::function<void(ViewNode*)> callback) override;
};
using DyldSharedCacheViewNodePtr = std::shared_ptr<DyldSharedCacheViewNode>;

MOEX_NAMESPACE_END

#endif // MOEX_DYLDSHAREDCACHEVIEWNODE_H
