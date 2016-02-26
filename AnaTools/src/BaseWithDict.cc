#include "OSUT3Analysis/AnaTools/interface/BaseWithDict.h"

#include "OSUT3Analysis/AnaTools/interface/TypeWithDict.h"

#include "TBaseClass.h"

namespace anatools {

  BaseWithDict::BaseWithDict() : baseClass_(nullptr) {
  }

  BaseWithDict::BaseWithDict(TBaseClass* baseClass) : baseClass_(baseClass) {
  }

  bool
  BaseWithDict::isPublic() const {
    return baseClass_->Property() & kIsPublic;
  }

  std::string
  BaseWithDict::name() const {
    return baseClass_->GetName();
  }

  TypeWithDict
  BaseWithDict::typeOf() const {
    return TypeWithDict(baseClass_->GetClassPointer());
  }

  size_t
  BaseWithDict::offset() const {
    return static_cast<size_t>(baseClass_->GetDelta());
  }

} // namespace anatools
