/* 
 * File:   comments_location.h
 * Author: maxim
 *
 * Created on 9 Июль 2014 г., 17:15
 */

#ifndef COMMENTS_LOCATION_H
#define	COMMENTS_LOCATION_H

#include <clang/Basic/SourceManager.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Lex/Preprocessor.h>

#include <vector>

#include "Anno/anno.h"
#include "Util/util.h"

namespace borealis {
namespace comments {

typedef borealis::anno::command command;

class GatherCommentsSourceLocationAction: public clang::PreprocessOnlyAction {

public:

    typedef std::vector<std::pair<clang::SourceRange, command>> comment_container;
    typedef comment_container::iterator iterator;
    typedef std::pair<iterator, iterator> range;

private:

    class CommentKeeper: public virtual clang::CommentHandler {
        clang::Preprocessor& pp;
        comment_container comments;
        bool attached;

    public:
        CommentKeeper(clang::Preprocessor& ppi): pp(ppi), attached(false) {};

        virtual bool HandleComment(clang::Preprocessor &PP, clang::SourceRange Comment);
        virtual ~CommentKeeper() { detach(); };

        void attach() {
            if (!attached) pp.AddCommentHandler(this);
            attached = true;
        }

        comment_container detach() {
            if (attached) pp.RemoveCommentHandler(this);
            attached = false;
            return std::move(comments);
        }
    };

    std::unique_ptr<CommentKeeper> keeper;
    llvm::StringRef currentFile = "";

    comment_container allComments;

public:
    const comment_container& getComments() const {
        return allComments;
    }

protected:
    virtual bool BeginSourceFileAction(
            clang::CompilerInstance &CI,
            llvm::StringRef Filename) {
        auto& PP = CI.getPreprocessor();
        currentFile = Filename;
        keeper.reset(new CommentKeeper(PP));
        keeper->attach();
        return true;
    }

    virtual void EndSourceFileAction() {
        auto fl = keeper->detach();
        allComments.insert(allComments.end(), fl.begin(), fl.end());
    }
};

} // namespace comments
} // namespace borealis

#endif	/* COMMENTS_LOCATION_H */

