//
// Created by abdullin on 2/4/19.
//

#ifndef BOREALIS_STRUCTDOMAIN_HPP
#define BOREALIS_STRUCTDOMAIN_HPP

#include <unordered_map>

#include "Interpreter/Domain/DomainFactory.h"

#include "Util/sayonara.hpp"
#include "Util/macros.h"

namespace borealis {
namespace absint {

template <typename MachineInt>
class StructLocation;

template <typename MachineInt>
class StructDomain : public AbstractDomain {
public:

    using Ptr = AbstractDomain::Ptr;
    using ConstPtr = AbstractDomain::ConstPtr;

    using Self = StructDomain<MachineInt>;
    using IntervalT = Interval<MachineInt>;
    using MemLocationT = StructLocation<MachineInt>;
    using PointerT = Pointer<MachineInt>;
    using Types = std::vector<Type::Ptr>;
    using Elements = std::vector<Ptr>;

private:

    Types types_;
    AbstractFactory* factory_;
    Elements elements_;

private:
    struct TopTag{};
    struct BottomTag{};

    explicit StructDomain(TopTag, const Types& types) :
            types_(types), factory_(AbstractFactory::get()) {
        for (auto i = 0U; i < types_.size(); ++i) {
            elements_.push_back(factory_->top(types[i]));
        }
    }

    explicit StructDomain(BottomTag, const Types& types) :
            types_(types), factory_(AbstractFactory::get()) {
        for (auto i = 0U; i < types_.size(); ++i) {
            elements_.push_back(factory_->bottom(types[i]));
        }
    }

    static Self* unwrap(Ptr other) {
        auto* otherRaw = llvm::dyn_cast<Self>(other.get());
        ASSERTC(otherRaw);

        return otherRaw;
    }

    static const Self* unwrap(ConstPtr other) {
        auto* otherRaw = llvm::dyn_cast<const Self>(other.get());
        ASSERTC(otherRaw);

        return otherRaw;
    }

public:

    explicit StructDomain(const Types& types) : StructDomain(TopTag{}, types) {}
    StructDomain(const Types& types, const Elements& elements) : AbstractDomain(class_tag(*this)), types_(types), elements_(elements) {}
    StructDomain(const StructDomain&) = default;
    StructDomain(StructDomain&&) = default;
    StructDomain& operator=(const StructDomain&) = default;
    StructDomain& operator=(StructDomain&&) = default;

    static Ptr top(const Types& types) { return std::make_shared<StructDomain>(TopTag{}, types); }
    static Ptr bottom(const Types& types) { return std::make_shared<StructDomain>(BottomTag{}, types); }

    const Types& types() const { return types_; }

    size_t size() const {
        return elements_.size();
    }

    bool isTop() const override {
        bool top = true;
        for (auto i = 0U; i < size(); ++i) {
            top &= elements_[i]->isTop();
        }
        return top;
    }

    bool isBottom() const override {
        bool top = true;
        for (auto i = 0U; i < size(); ++i) {
            top &= elements_[i]->isBottom();
        }
        return top;
    }

    void setTop() override {
        for (auto i = 0U; i < size(); ++i) {
            elements_[i]->setTop();
        }
    }

    void setBottom() override {
        for (auto i = 0U; i < size(); ++i) {
            elements_[i]->setBottom();
        }
    }

    bool leq(ConstPtr other) const override {
        if (this->isBottom()) {
            return true;
        } else if (other->isBottom()) {
            return false;
        } else {
            UNREACHABLE("Don't know how to implement");
        }
    }

    bool equals(ConstPtr other) const  override {
        auto* otherRaw = unwrap(other);

        if (this->isBottom()) {
            return other->isBottom();
        } else if (this->isTop()) {
            return other->isTop();
        } else if (other->isTop() || other->isBottom()) {
            return false;
        } else {

            if (this->size() != otherRaw->size()) return false;

            for (auto i = 0U; i < this->length_; ++i) {
                if (this->elements_[i] != otherRaw->elements_[i]) {
                    return false;
                }
            }

            return true;
        }
    }

    void joinWith(ConstPtr other) override {
        auto* otherRaw = unwrap(other);

        if (this->isBottom()) {
            this->operator=(*otherRaw);
        } else if (this->isTop()) {
            return;
        } else if (other->isBottom()) {
            return;
        } else if (other->isTop()) {
            this->operator=(*otherRaw);
        } else {

            ASSERT(this->size() == otherRaw->size(), "trying to join different length structs");

            for (auto i = 0U; i < this->length_; ++i) {
                this->elements_[i]->joinWith(otherRaw->elements_[i]);
            }
        }
    }

    void meetWith(ConstPtr other) override {
        auto* otherRaw = unwrap(other);

        if (this->isBottom()) {
            return;
        } else if (this->isTop()) {
            this->operator=(*otherRaw);
        } else if (other->isBottom()) {
            this->operator=(*otherRaw);
        } else if (other->isTop()) {
            return;
        } else {

            ASSERT(this->size() == otherRaw->size(), "trying to meet different length structs");

            for (auto i = 0U; i < this->length_; ++i) {
                this->elements_[i]->meetWith(otherRaw->elements_[i]);
            }
        }
    }

    void widenWith(ConstPtr other) override {
        auto* otherRaw = unwrap(other);

        if (this->isBottom()) {
            this->operator=(*otherRaw);
        } else if (this->isTop()) {
            return;
        } else if (other->isBottom()) {
            return;
        } else if (other->isTop()) {
            this->operator=(*otherRaw);
        } else {

            ASSERT(this->size() == otherRaw->size(), "trying to widen different length structs");

            for (auto i = 0U; i < this->length_; ++i) {
                this->elements_[i]->widenWith(otherRaw->elements_[i]);
            }
        }
    }

    size_t hashCode() const override {
        return util::hash::defaultHasher()(this->length_, this->elements_);
    }

    std::string toString() const override {
        std::ostringstream ss;
        ss << "Struct [";
        if (not types_.empty()) {
            ss << TypeUtils::toString(*types_[0].get());
        }
        for (auto i = 1U; i < this->length_; ++i) {
            ss << ", " << TypeUtils::toString(*types_[i].get());
        }
        ss << "] ";
        ss << ": {";
        if (this->isTop()) {
            ss << " TOP }";
        } else if (this->isBottom()) {
            ss << " BOTTOM }";
        } else {
            for (auto i = 0U; i < this->length_; ++i) {
                ss << std::endl << "  " << this->elements_[i]->toString();
            }
            ss << std::endl << "}";
        }
        return ss.str();
    }

    Ptr load(Type::Ptr, Ptr interval) const override {
        auto* intervalRaw = llvm::dyn_cast<IntervalT>(interval.get());
        ASSERTC(intervalRaw);
        ASSERT(intervalRaw->isConstant(), "trying to load from nondetermined index of struct");

        auto index = intervalRaw->asConstant();
        auto type = types_[index];

        if (this->isTop()) {
            return factory_->top(type);
        } else if (this->isBottom()) {
            return factory_->bottom(type);
        } else {
            ASSERTC(index <= size());
            return elements_[index];
        }
    }

    void store(Ptr value, Ptr interval) override {
        auto* intervalRaw = llvm::dyn_cast<IntervalT>(interval.get());
        ASSERTC(intervalRaw);
        ASSERT(intervalRaw->isConstant(), "trying to load from nondetermined index of struct");
        auto index = intervalRaw->asConstant();

        if (this->isTop()) {
            return;
        } else {
            elements_[index]->joinWith(value);
        }
    }

    Ptr gep(Type::Ptr type, const std::vector<ConstPtr>& offsets) const override {
        if (this->isTop()) {
            return PointerT::top(type);
        } else if (this->isBottom()) {
            return PointerT::bottom(type);
        }

        auto* intervalRaw = llvm::dyn_cast<IntervalT>(offsets[0].get());
        ASSERTC(intervalRaw);
        ASSERT(intervalRaw->isConstant(), "trying to load from nondetermined index of struct");
        auto index = intervalRaw->asConstant();

        if (offsets.size() == 1) {
            return PointerT(type, std::make_shared<MemLocationT>(shared_from_this(), offsets[0]));
        } else {
            return elements_[index]->gep(type, std::vector<ConstPtr>(offsets.begin() + 1, offsets.end()));
        }
    }
};

} // namespace absint
} // namespace borealis

#include "Util/unmacros.h"

#endif //BOREALIS_STRUCTDOMAIN_HPP
