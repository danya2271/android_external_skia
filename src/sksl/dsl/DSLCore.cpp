/*
 * Copyright 2020 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/sksl/dsl/DSLCore.h"

#include "src/sksl/SkSLCompiler.h"
#include "src/sksl/SkSLIRGenerator.h"
#include "src/sksl/dsl/priv/DSLWriter.h"
#include "src/sksl/ir/SkSLDoStatement.h"
#include "src/sksl/ir/SkSLForStatement.h"
#include "src/sksl/ir/SkSLIfStatement.h"

namespace SkSL {

namespace dsl {

#if SK_SUPPORT_GPU && !defined(SKSL_STANDALONE)
void Start(SkSL::Compiler* compiler) {
    DSLWriter::SetInstance(std::make_unique<DSLWriter>(compiler));
}

void End() {
    DSLWriter::SetInstance(nullptr);
}
#endif // SK_SUPPORT_GPU && !defined(SKSL_STANDALONE)

void SetErrorHandler(ErrorHandler* errorHandler) {
    DSLWriter::SetErrorHandler(errorHandler);
}
class DSLCore {
public:
    template <typename... Args>
    static DSLExpression Call(const char* name, Args... args) {
        SkSL::IRGenerator& ir = DSLWriter::IRGenerator();
        SkSL::ExpressionArray argArray;
        argArray.reserve_back(sizeof...(args));

        // in C++17, we could just do:
        // (argArray.push_back(args.release()), ...);
        int unused[] = {0, (ignore(argArray.push_back(args.release())), 0)...};
        static_cast<void>(unused);

        return ir.call(/*offset=*/-1, ir.convertIdentifier(-1, name), std::move(argArray));
    }

    static DSLStatement Declare(DSLVar& var, DSLExpression initialValue) {
        if (!var.fDeclaration) {
            DSLWriter::ReportError("Declare failed (was the variable already declared?)");
            return DSLStatement();
        }
        VarDeclaration& decl = var.fDeclaration->as<SkSL::VarDeclaration>();
        std::unique_ptr<Expression> expr = initialValue.coerceAndRelease(decl.var().type());
        if (expr) {
            decl.fValue = std::move(expr);
        }
        return DSLStatement(std::move(var.fDeclaration));
    }

    static DSLStatement Do(DSLStatement stmt, DSLExpression test) {
        return DSLWriter::IRGenerator().convertDo(stmt.release(), test.release());
    }

    static DSLStatement For(DSLStatement initializer, DSLExpression test, DSLExpression next,
                            DSLStatement stmt) {
        return DSLWriter::IRGenerator().convertFor(/*offset=*/-1, initializer.release(),
                                                   test.release(), next.release(), stmt.release());
    }

    static DSLStatement If(DSLExpression test, DSLStatement ifTrue, DSLStatement ifFalse) {
        return DSLWriter::IRGenerator().convertIf(/*offset=*/-1, /*isStatic=*/false, test.release(),
                                                  ifTrue.release(), ifFalse.release());
    }

    static DSLExpression Ternary(DSLExpression test, DSLExpression ifTrue, DSLExpression ifFalse) {
        return DSLWriter::IRGenerator().convertTernaryExpression(test.release(), ifTrue.release(),
                                                                 ifFalse.release());
    }

    static DSLStatement While(DSLExpression test, DSLStatement stmt) {
        return DSLWriter::IRGenerator().convertWhile(/*offset=*/-1, test.release(), stmt.release());
    }

private:
    static void ignore(std::unique_ptr<SkSL::Expression>&) {}
};

DSLStatement Declare(DSLVar& var, DSLExpression initialValue) {
    return DSLCore::Declare(var, std::move(initialValue));
}

DSLStatement Do(DSLStatement stmt, DSLExpression test) {
    return DSLCore::Do(std::move(stmt), std::move(test));
}

DSLStatement For(DSLStatement initializer, DSLExpression test, DSLExpression next,
                 DSLStatement stmt) {
    return DSLCore::For(std::move(initializer), std::move(test), std::move(next), std::move(stmt));
}

DSLStatement If(DSLExpression test, DSLStatement ifTrue, DSLStatement ifFalse) {
    return DSLCore::If(std::move(test), std::move(ifTrue), std::move(ifFalse));
}

DSLExpression Ternary(DSLExpression test, DSLExpression ifTrue, DSLExpression ifFalse) {
    return DSLCore::Ternary(std::move(test), std::move(ifTrue), std::move(ifFalse));
}

DSLStatement While(DSLExpression test, DSLStatement stmt) {
    return DSLCore::While(std::move(test), std::move(stmt));
}

DSLExpression Abs(DSLExpression x) {
    return DSLCore::Call("abs", std::move(x));
}

DSLExpression All(DSLExpression x) {
    return DSLCore::Call("all", std::move(x));
}

DSLExpression Any(DSLExpression x) {
    return DSLCore::Call("any", std::move(x));
}

DSLExpression Ceil(DSLExpression x) {
    return DSLCore::Call("ceil", std::move(x));
}

DSLExpression Clamp(DSLExpression x, DSLExpression min, DSLExpression max) {
    return DSLCore::Call("clamp", std::move(x), std::move(min), std::move(max));
}

DSLExpression Cos(DSLExpression x) {
    return DSLCore::Call("cos", std::move(x));
}

DSLExpression Cross(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("cross", std::move(x), std::move(y));
}

DSLExpression Degrees(DSLExpression x) {
    return DSLCore::Call("degrees", std::move(x));
}

DSLExpression Distance(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("distance", std::move(x), std::move(y));
}

DSLExpression Dot(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("dot", std::move(x), std::move(y));
}

DSLExpression Equal(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("equal", std::move(x), std::move(y));
}

DSLExpression Exp(DSLExpression x) {
    return DSLCore::Call("exp", std::move(x));
}

DSLExpression Exp2(DSLExpression x) {
    return DSLCore::Call("exp2", std::move(x));
}

DSLExpression Faceforward(DSLExpression n, DSLExpression i, DSLExpression nref) {
    return DSLCore::Call("faceforward", std::move(n), std::move(i), std::move(nref));
}

DSLExpression Fract(DSLExpression x) {
    return DSLCore::Call("fract", std::move(x));
}

DSLExpression Floor(DSLExpression x) {
    return DSLCore::Call("floor", std::move(x));
}

DSLExpression GreaterThan(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("greaterThan", std::move(x), std::move(y));
}

DSLExpression GreaterThanEqual(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("greaterThanEqual", std::move(x), std::move(y));
}

DSLExpression Inverse(DSLExpression x) {
    return DSLCore::Call("inverse", std::move(x));
}

DSLExpression Inversesqrt(DSLExpression x) {
    return DSLCore::Call("inversesqrt", std::move(x));
}

DSLExpression Length(DSLExpression x) {
    return DSLCore::Call("length", std::move(x));
}

DSLExpression LessThan(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("lessThan", std::move(x), std::move(y));
}

DSLExpression LessThanEqual(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("lessThanEqual", std::move(x), std::move(y));
}

DSLExpression Log(DSLExpression x) {
    return DSLCore::Call("log", std::move(x));
}

DSLExpression Log2(DSLExpression x) {
    return DSLCore::Call("log2", std::move(x));
}

DSLExpression Max(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("max", std::move(x), std::move(y));
}

DSLExpression Min(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("min", std::move(x), std::move(y));
}

DSLExpression Mix(DSLExpression x, DSLExpression y, DSLExpression a) {
    return DSLCore::Call("mix", std::move(x), std::move(y), std::move(a));
}

DSLExpression Mod(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("mod", std::move(x), std::move(y));
}

DSLExpression Normalize(DSLExpression x) {
    return DSLCore::Call("normalize", std::move(x));
}

DSLExpression NotEqual(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("notEqual", std::move(x), std::move(y));
}

DSLExpression Pow(DSLExpression x, DSLExpression y) {
    return DSLCore::Call("pow", std::move(x), std::move(y));
}

DSLExpression Radians(DSLExpression x) {
    return DSLCore::Call("radians", std::move(x));
}

DSLExpression Reflect(DSLExpression i, DSLExpression n) {
    return DSLCore::Call("reflect", std::move(i), std::move(n));
}

DSLExpression Refract(DSLExpression i, DSLExpression n, DSLExpression eta) {
    return DSLCore::Call("refract", std::move(i), std::move(n), std::move(eta));
}

DSLExpression Saturate(DSLExpression x) {
    return DSLCore::Call("saturate", std::move(x));
}

DSLExpression Sign(DSLExpression x) {
    return DSLCore::Call("sign", std::move(x));
}

DSLExpression Sin(DSLExpression x) {
    return DSLCore::Call("sin", std::move(x));
}

DSLExpression Smoothstep(DSLExpression edge1, DSLExpression edge2, DSLExpression x) {
    return DSLCore::Call("smoothstep", std::move(edge1), std::move(edge2), std::move(x));
}

DSLExpression Sqrt(DSLExpression x) {
    return DSLCore::Call("sqrt", std::move(x));
}

DSLExpression Step(DSLExpression edge, DSLExpression x) {
    return DSLCore::Call("step", std::move(edge), std::move(x));
}

DSLExpression Tan(DSLExpression x) {
    return DSLCore::Call("tan", std::move(x));
}

DSLExpression Unpremul(DSLExpression x) {
    return DSLCore::Call("unpremul", std::move(x));
}

} // namespace dsl

} // namespace SkSL
