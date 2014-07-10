/* 
 * File:   comments_location.cpp
 * Author: maxim
 * 
 * Created on 9 Июль 2014 г., 17:15
 */

#include <utility>

#include "Actions/comments_location.h"

#include "Util/macros.h"

namespace borealis {
namespace comments {

typedef GatherCommentsSourceLocationAction::comment_container comment_container;

static comment_container getRawTextSlow(const clang::SourceManager &SourceMgr, clang::SourceRange Range) {
    if (SourceMgr.isInSystemHeader(Range.getBegin())) {
        return comment_container();
    }

    clang::FileID BeginFileID;
    clang::FileID EndFileID;
    unsigned BeginOffset;
    unsigned EndOffset;

    std::tie(BeginFileID, BeginOffset) =
       SourceMgr.getDecomposedLoc(Range.getBegin());
    std::tie(EndFileID, EndOffset) =
       SourceMgr.getDecomposedLoc(Range.getEnd());

    const unsigned Length = EndOffset - BeginOffset;
    if (Length < 2) return comment_container();

    ASSERT(BeginFileID == EndFileID,
           "Comment can't begin in one file and end in another one");

    bool Invalid = false;
    const char* BufferStart =
        SourceMgr.getBufferData(BeginFileID, &Invalid).data();
    if (Invalid) return comment_container();

    auto comment = llvm::StringRef(BufferStart + BeginOffset, Length);
    auto commands = borealis::anno::parse(comment.str());

    auto ret = comment_container();
    for (auto& cmd : commands) {
        ret.emplace_back(Range, std::move(cmd));
    }

    return ret;
}

bool GatherCommentsSourceLocationAction::CommentKeeper::HandleComment(clang::Preprocessor &PP, clang::SourceRange Comment) {
    clang::SourceManager& sm = PP.getSourceManager();
    auto raw = getRawTextSlow(sm, Comment);
    comments.insert(comments.end(), raw.begin(), raw.end());
    return false;
}

} // namespace comments
} // namespace borealis

#include "Util/unmacros.h"