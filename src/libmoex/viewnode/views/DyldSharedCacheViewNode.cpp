#include "DyldSharedCacheViewNode.h"

MOEX_NAMESPACE_BEGIN

void DyldSharedCacheMappingsViewNode::InitViewDatas()
{
    using namespace moex::util;
    if (!cache_) return;
    auto t = CreateTableView();
    t->SetHeaders({"Address", "Size", "File Offset", "MaxProt", "InitProt"});
    t->SetWidths({180, 180, 180, 120, 120});
    for (const auto &m : cache_->mappings()) {
        t->AddRow({AsAddress(m.address), AsAddress(m.size), AsAddress(m.fileOffset),
                   AsShortHexString(m.maxProt), AsShortHexString(m.initProt)});
    }
}

void DyldSharedCacheImagesViewNode::InitViewDatas()
{
    using namespace moex::util;
    if (!cache_) return;
    auto t = CreateTableView();
    t->SetHeaders({"Address", "Path Offset", "Path"});
    t->SetWidths({180, 180, 620});
    uint32_t idx = 0;
    if (!cache_->images().empty()) {
        for (const auto &img : cache_->images()) {
            std::string path = cache_->ReadPathByOffset(img.pathFileOffset);
            t->AddRow({AsAddress(img.address), AsAddress(img.pathFileOffset), path});
            if (++idx >= 5000) {
                t->AddRow({"-", "-", "(truncated at 5000 entries)"});
                break;
            }
        }
    } else {
        for (const auto &img : cache_->map_images()) {
            t->AddRow({AsAddress(img.first), "-", img.second});
            if (++idx >= 5000) {
                t->AddRow({"-", "-", "(truncated at 5000 entries)"});
                break;
            }
        }
    }
}

void DyldSharedCacheViewNode::Init(DyldSharedCachePtr cache)
{
    cache_ = cache;
    mappings_ = std::make_shared<DyldSharedCacheMappingsViewNode>();
    mappings_->Init(cache);
    images_ = std::make_shared<DyldSharedCacheImagesViewNode>();
    images_->Init(cache);
}

void DyldSharedCacheViewNode::InitViewDatas()
{
    using namespace moex::util;
    if (!cache_) return;
    auto t = CreateTableView();
    const auto &h = cache_->header();
    t->AddRow((void*)h.magic, 16, "Magic", std::string(h.magic, h.magic + 16).c_str());
    t->AddRow(h.mappingOffset, "Mapping Offset", AsShortHexString(h.mappingOffset));
    t->AddRow(h.mappingCount, "Mapping Count", AsString(h.mappingCount));
    t->AddRow(h.imagesOffset, "Images Offset", AsShortHexString(h.imagesOffset));
    t->AddRow(h.imagesCount, "Images Count", AsString(h.imagesCount));
    t->AddRow(h.dyldBaseAddress, "Dyld Base Address", AsShortHexString(h.dyldBaseAddress));
    t->AddRow(h.localSymbolsOffset, "Local Symbols Offset", AsShortHexString(h.localSymbolsOffset));
    t->AddRow(h.localSymbolsSize, "Local Symbols Size", AsShortHexString(h.localSymbolsSize));
    t->AddRow(h.codeSignatureOffset, "Code Signature Offset", AsShortHexString(h.codeSignatureOffset));
    t->AddRow(h.codeSignatureSize, "Code Signature Size", AsShortHexString(h.codeSignatureSize));

    auto b = CreateBinaryView();
    b->offset = cache_->base();
    b->size = cache_->ctx()->file_size;
    b->start_value = 0;
}

void DyldSharedCacheViewNode::ForEachChild(std::function<void(ViewNode *)> callback)
{
    if (mappings_) callback(mappings_.get());
    if (images_) callback(images_.get());
}

MOEX_NAMESPACE_END
