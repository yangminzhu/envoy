#ifndef ENVOY_CEL_AST_H
#define ENVOY_CEL_AST_H

#include <string>
#include "test/cel/cel_ast.pb.h"

cel::AttributesContext AttributesContext();
std::string CELAst();
std::string CELAstFlattenedMap();
std::string RBACFilterConfig();

#endif //ENVOY_CEL_AST_H
