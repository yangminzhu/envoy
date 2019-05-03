#include "test/cel/cel_ast.h"
#include "test/cel/cel_ast.pb.h"

#include <string>

cel::AttributesContext AttributesContext() {
  cel::AttributesContext ac;
  auto headers = ac.mutable_headers();
  (*headers)["ip"] = "10.0.1.2";
  (*headers)["path"] = "/admin/edit";
  (*headers)["token"] = "admin";

  //  std::string dump;
  //  TextFormat::PrintToString(ac, &dump);
  //  std::cout << dump << "\n";
  return ac;
}

std::string CELAst() {
/*
 !(headers.ip in ["10.0.1.4", "10.0.1.5", "10.0.1.6"]) &&
   (
    (headers.path.startsWith("v1") && headers.token in ["v1", "v2", "admin"]) ||
    (headers.path.startsWith("v2") && headers.token in ["v2", "admin"]) ||
    (headers.path.startsWith("/admin") && headers.token == "admin" && headers.ip in ["10.0.1.1", "10.0.1.2", "10.0.1.3"])
   )
*/
  return R"(
id: 51
call_expr: <
  function: "_&&_"
  args: <
    id: 1
    call_expr: <
      function: "!_"
      args: <
        id: 4
        call_expr: <
          function: "@in"
          args: <
            id: 3
            select_expr: <
              operand: <
                id: 2
                ident_expr: <
                  name: "headers"
                >
              >
              field: "ip"
            >
          >
          args: <
            id: 5
            list_expr: <
              elements: <
                id: 6
                const_expr: <
                  string_value: "10.0.1.4"
                >
              >
              elements: <
                id: 7
                const_expr: <
                  string_value: "10.0.1.5"
                >
              >
              elements: <
                id: 8
                const_expr: <
                  string_value: "10.0.1.6"
                >
              >
            >
          >
        >
      >
    >
  >
  args: <
    id: 50
    call_expr: <
      function: "_||_"
      args: <
        id: 32
        call_expr: <
          function: "_||_"
          args: <
            id: 20
            call_expr: <
              function: "_&&_"
              args: <
                id: 11
                call_expr: <
                  target: <
                    id: 10
                    select_expr: <
                      operand: <
                        id: 9
                        ident_expr: <
                          name: "headers"
                        >
                      >
                      field: "path"
                    >
                  >
                  function: "startsWith"
                  args: <
                    id: 12
                    const_expr: <
                      string_value: "v1"
                    >
                  >
                >
              >
              args: <
                id: 15
                call_expr: <
                  function: "@in"
                  args: <
                    id: 14
                    select_expr: <
                      operand: <
                        id: 13
                        ident_expr: <
                          name: "headers"
                        >
                      >
                      field: "token"
                    >
                  >
                  args: <
                    id: 16
                    list_expr: <
                      elements: <
                        id: 17
                        const_expr: <
                          string_value: "v1"
                        >
                      >
                      elements: <
                        id: 18
                        const_expr: <
                          string_value: "v2"
                        >
                      >
                      elements: <
                        id: 19
                        const_expr: <
                          string_value: "admin"
                        >
                      >
                    >
                  >
                >
              >
            >
          >
          args: <
            id: 31
            call_expr: <
              function: "_&&_"
              args: <
                id: 23
                call_expr: <
                  target: <
                    id: 22
                    select_expr: <
                      operand: <
                        id: 21
                        ident_expr: <
                          name: "headers"
                        >
                      >
                      field: "path"
                    >
                  >
                  function: "startsWith"
                  args: <
                    id: 24
                    const_expr: <
                      string_value: "v2"
                    >
                  >
                >
              >
              args: <
                id: 27
                call_expr: <
                  function: "@in"
                  args: <
                    id: 26
                    select_expr: <
                      operand: <
                        id: 25
                        ident_expr: <
                          name: "headers"
                        >
                      >
                      field: "token"
                    >
                  >
                  args: <
                    id: 28
                    list_expr: <
                      elements: <
                        id: 29
                        const_expr: <
                          string_value: "v2"
                        >
                      >
                      elements: <
                        id: 30
                        const_expr: <
                          string_value: "admin"
                        >
                      >
                    >
                  >
                >
              >
            >
          >
        >
      >
      args: <
        id: 49
        call_expr: <
          function: "_&&_"
          args: <
            id: 41
            call_expr: <
              function: "_&&_"
              args: <
                id: 35
                call_expr: <
                  target: <
                    id: 34
                    select_expr: <
                      operand: <
                        id: 33
                        ident_expr: <
                          name: "headers"
                        >
                      >
                      field: "path"
                    >
                  >
                  function: "startsWith"
                  args: <
                    id: 36
                    const_expr: <
                      string_value: "/admin"
                    >
                  >
                >
              >
              args: <
                id: 39
                call_expr: <
                  function: "_==_"
                  args: <
                    id: 38
                    select_expr: <
                      operand: <
                        id: 37
                        ident_expr: <
                          name: "headers"
                        >
                      >
                      field: "token"
                    >
                  >
                  args: <
                    id: 40
                    const_expr: <
                      string_value: "admin"
                    >
                  >
                >
              >
            >
          >
          args: <
            id: 44
            call_expr: <
              function: "@in"
              args: <
                id: 43
                select_expr: <
                  operand: <
                    id: 42
                    ident_expr: <
                      name: "headers"
                    >
                  >
                  field: "ip"
                >
              >
              args: <
                id: 45
                list_expr: <
                  elements: <
                    id: 46
                    const_expr: <
                      string_value: "10.0.1.1"
                    >
                  >
                  elements: <
                    id: 47
                    const_expr: <
                      string_value: "10.0.1.2"
                    >
                  >
                  elements: <
                    id: 48
                    const_expr: <
                      string_value: "10.0.1.3"
                    >
                  >
                >
              >
            >
          >
        >
      >
    >
  >
>
)";
}

std::string CELAstFlattenedMap() {
  /*
 !(ip in ["10.0.1.4", "10.0.1.5", "10.0.1.6"]) &&
   (
    (path.startsWith("v1") && token in ["v1", "v2", "admin"]) ||
    (path.startsWith("v2") && token in ["v2", "admin"]) ||
    (path.startsWith("/admin") && token == "admin" && ip in ["10.0.1.1", "10.0.1.2", "10.0.1.3"])
   )
*/
  return R"(
id: 43
call_expr: <
  function: "_&&_"
  args: <
    id: 1
    call_expr: <
      function: "!_"
      args: <
        id: 3
        call_expr: <
          function: "@in"
          args: <
            id: 2
            ident_expr: <
              name: "ip"
            >
          >
          args: <
            id: 4
            list_expr: <
              elements: <
                id: 5
                const_expr: <
                  string_value: "10.0.1.4"
                >
              >
              elements: <
                id: 6
                const_expr: <
                  string_value: "10.0.1.5"
                >
              >
              elements: <
                id: 7
                const_expr: <
                  string_value: "10.0.1.6"
                >
              >
            >
          >
        >
      >
    >
  >
  args: <
    id: 42
    call_expr: <
      function: "_||_"
      args: <
        id: 27
        call_expr: <
          function: "_||_"
          args: <
            id: 17
            call_expr: <
              function: "_&&_"
              args: <
                id: 9
                call_expr: <
                  target: <
                    id: 8
                    ident_expr: <
                      name: "path"
                    >
                  >
                  function: "startsWith"
                  args: <
                    id: 10
                    const_expr: <
                      string_value: "v1"
                    >
                  >
                >
              >
              args: <
                id: 12
                call_expr: <
                  function: "@in"
                  args: <
                    id: 11
                    ident_expr: <
                      name: "token"
                    >
                  >
                  args: <
                    id: 13
                    list_expr: <
                      elements: <
                        id: 14
                        const_expr: <
                          string_value: "v1"
                        >
                      >
                      elements: <
                        id: 15
                        const_expr: <
                          string_value: "v2"
                        >
                      >
                      elements: <
                        id: 16
                        const_expr: <
                          string_value: "admin"
                        >
                      >
                    >
                  >
                >
              >
            >
          >
          args: <
            id: 26
            call_expr: <
              function: "_&&_"
              args: <
                id: 19
                call_expr: <
                  target: <
                    id: 18
                    ident_expr: <
                      name: "path"
                    >
                  >
                  function: "startsWith"
                  args: <
                    id: 20
                    const_expr: <
                      string_value: "v2"
                    >
                  >
                >
              >
              args: <
                id: 22
                call_expr: <
                  function: "@in"
                  args: <
                    id: 21
                    ident_expr: <
                      name: "token"
                    >
                  >
                  args: <
                    id: 23
                    list_expr: <
                      elements: <
                        id: 24
                        const_expr: <
                          string_value: "v2"
                        >
                      >
                      elements: <
                        id: 25
                        const_expr: <
                          string_value: "admin"
                        >
                      >
                    >
                  >
                >
              >
            >
          >
        >
      >
      args: <
        id: 41
        call_expr: <
          function: "_&&_"
          args: <
            id: 34
            call_expr: <
              function: "_&&_"
              args: <
                id: 29
                call_expr: <
                  target: <
                    id: 28
                    ident_expr: <
                      name: "path"
                    >
                  >
                  function: "startsWith"
                  args: <
                    id: 30
                    const_expr: <
                      string_value: "/admin"
                    >
                  >
                >
              >
              args: <
                id: 32
                call_expr: <
                  function: "_==_"
                  args: <
                    id: 31
                    ident_expr: <
                      name: "token"
                    >
                  >
                  args: <
                    id: 33
                    const_expr: <
                      string_value: "admin"
                    >
                  >
                >
              >
            >
          >
          args: <
            id: 36
            call_expr: <
              function: "@in"
              args: <
                id: 35
                ident_expr: <
                  name: "ip"
                >
              >
              args: <
                id: 37
                list_expr: <
                  elements: <
                    id: 38
                    const_expr: <
                      string_value: "10.0.1.1"
                    >
                  >
                  elements: <
                    id: 39
                    const_expr: <
                      string_value: "10.0.1.2"
                    >
                  >
                  elements: <
                    id: 40
                    const_expr: <
                      string_value: "10.0.1.3"
                    >
                  >
                >
              >
            >
          >
        >
      >
    >
  >
>

)";
}

std::string RBACFilterConfig() {
/*
apiVersion: "rbac.istio.io/v1alpha1"
kind: AuthorizationPolicy
metadata:
  name: benchmark
spec:
  workloadSelector:
    labels:
        app: sleep
  allow:
  - subjects:
    - names: ["allUsers"]
    actions:
    - paths: ["v1*"]
      not_hosts: ["10.0.1.4", "10.0.1.5", "10.0.1.6"]
      constraints:
      - key: "request.headers[token]"
        values: ["v1", "v2", "admin"]
  - subjects:
    - names: ["allUsers"]
    actions:
    - paths: ["v2*"]
      not_hosts: ["10.0.1.4", "10.0.1.5", "10.0.1.6"]
      constraints:
      - key: "request.headers[token]"
        values: ["v2", "admin"]
  - subjects:
    - names: ["allUsers"]
    actions:
    - paths: ["/admin*"]
      not_hosts: ["10.0.1.4", "10.0.1.5", "10.0.1.6"]
      constraints:
      - key: "request.headers[token]"
        values: ["admin"]
*/
  return R"(
policies: <
  key: "authz-policy-benchmark-allow[0]"
  value: <
    permissions: <
      and_rules: <
        rules: <
          not_rule: <
            or_rules: <
              rules: <
                header: <
                  name: ":authority"
                  exact_match: "10.0.1.4"
                >
              >
              rules: <
                header: <
                  name: ":authority"
                  exact_match: "10.0.1.5"
                >
              >
              rules: <
                header: <
                  name: ":authority"
                  exact_match: "10.0.1.6"
                >
              >
            >
          >
        >
        rules: <
          or_rules: <
            rules: <
              header: <
                name: ":path"
                prefix_match: "v1"
              >
            >
          >
        >
        rules: <
          or_rules: <
            rules: <
              header: <
                name: "token"
                exact_match: "v1"
              >
            >
            rules: <
              header: <
                name: "token"
                exact_match: "v2"
              >
            >
            rules: <
              header: <
                name: "token"
                exact_match: "admin"
              >
            >
          >
        >
      >
    >
    principals: <
      and_ids: <
        ids: <
          or_ids: <
            ids: <
              any: true
            >
          >
        >
      >
    >
  >
>
policies: <
  key: "authz-policy-benchmark-allow[1]"
  value: <
    permissions: <
      and_rules: <
        rules: <
          not_rule: <
            or_rules: <
              rules: <
                header: <
                  name: ":authority"
                  exact_match: "10.0.1.4"
                >
              >
              rules: <
                header: <
                  name: ":authority"
                  exact_match: "10.0.1.5"
                >
              >
              rules: <
                header: <
                  name: ":authority"
                  exact_match: "10.0.1.6"
                >
              >
            >
          >
        >
        rules: <
          or_rules: <
            rules: <
              header: <
                name: ":path"
                prefix_match: "v2"
              >
            >
          >
        >
        rules: <
          or_rules: <
            rules: <
              header: <
                name: "token"
                exact_match: "v2"
              >
            >
            rules: <
              header: <
                name: "token"
                exact_match: "admin"
              >
            >
          >
        >
      >
    >
    principals: <
      and_ids: <
        ids: <
          or_ids: <
            ids: <
              any: true
            >
          >
        >
      >
    >
  >
>
policies: <
  key: "authz-policy-benchmark-allow[2]"
  value: <
    permissions: <
      and_rules: <
        rules: <
          not_rule: <
            or_rules: <
              rules: <
                header: <
                  name: ":authority"
                  exact_match: "10.0.1.4"
                >
              >
              rules: <
                header: <
                  name: ":authority"
                  exact_match: "10.0.1.5"
                >
              >
              rules: <
                header: <
                  name: ":authority"
                  exact_match: "10.0.1.6"
                >
              >
            >
          >
        >
        rules: <
          or_rules: <
            rules: <
              header: <
                name: ":path"
                prefix_match: "/admin"
              >
            >
          >
        >
        rules: <
          or_rules: <
            rules: <
              header: <
                name: "token"
                exact_match: "admin"
              >
            >
          >
        >
      >
    >
    principals: <
      and_ids: <
        ids: <
          or_ids: <
            ids: <
              any: true
            >
          >
        >
      >
    >
  >
>
)";
}