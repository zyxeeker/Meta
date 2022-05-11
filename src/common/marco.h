#ifndef SRC_COMMON_MARCO_H_
#define SRC_COMMON_MARCO_H_

#define DISALLOW_COPY_AND_ASSIGN(class_name) \
 private:                                    \
  class_name(const class_name&) = delete;    \
  class_name& operator=(const class_name&) = delete;

#endif  // SRC_COMMON_MARCO_H_
