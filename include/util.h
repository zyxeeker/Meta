/**
 * @author: zyxeeker
 * @createTime: 2023/02/07 3:33 AM
 * @description: ${description}
 */

#ifndef META_INCLUDE_UTIL_H_
#define META_INCLUDE_UTIL_H_

namespace meta {

#define SINGLE_DO_NOT_COPY(C)       \
C(T&&) = delete;                    \
C(const T&) = delete;               \
void operator=(const T&) = delete;

template <typename T>
class Single {
 public:
  static T& Instance() {
    return *_inst;
  }
  SINGLE_DO_NOT_COPY(Single)
 private:
  static T* _inst;
};

template<typename T>
T* Single<T>::_inst = new T;

} // namespace

#endif //META_INCLUDE_UTIL_H_
