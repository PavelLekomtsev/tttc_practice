// Лабораторная работа 1
// Вариант 3: Подсчет неявных преобразований типов в исходном коде
// Автор: Лекомцев Павел Олегович

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include <unordered_map>
#include <string>

using namespace clang;

class TypeConversionTracker {
public:
    void recordConversion(const std::string& functionName, 
                         const std::string& fromType, 
                         const std::string& toType) {
        std::string simplifiedFrom = simplifyTypeName(fromType);
        std::string simplifiedTo = simplifyTypeName(toType);
        std::string conversionKey = simplifiedFrom + " -> " + simplifiedTo;
        conversionStats[functionName][conversionKey]++;
    }

    void printResults(llvm::raw_ostream& out) const {
        for (const auto& [functionName, conversions] : conversionStats) {
            out << "Function `" << functionName << "`";
            for (const auto& [conversion, count] : conversions) {
                out << " " << conversion << ": " << count;
            }
            out << "\n";
        }
    }

private:
    std::string simplifyTypeName(const std::string& typeName) const {
        if (typeName == "int" || typeName == "const int") return "int";
        if (typeName == "float" || typeName == "const float") return "float";
        if (typeName == "double" || typeName == "const double") return "double";
        return typeName;
    }

    std::unordered_map<std::string, std::unordered_map<std::string, unsigned>> conversionStats;
};

class ImplicitCastVisitor : public RecursiveASTVisitor<ImplicitCastVisitor> {
public:
    ImplicitCastVisitor(ASTContext& context, TypeConversionTracker& tracker) 
        : context(context), tracker(tracker) {}

    bool VisitFunctionDecl(FunctionDecl* func) {
        currentFunction = func->hasBody() ? func->getNameAsString() : "";
        // llvm::outs() << "Анализ функции: " << CurrentFunction << "\n"; // для отладки
        return true;
    }

    bool VisitImplicitCastExpr(ImplicitCastExpr* cast) {
        if (currentFunction.empty() || shouldSkipCast(cast)) {
            return true;
        }

        QualType sourceType = cast->getSubExpr()->getType();
        QualType destType = cast->getType();

        tracker.recordConversion(currentFunction, 
                               sourceType.getAsString(), 
                               destType.getAsString());
        return true;
    }

private:
    bool shouldSkipCast(ImplicitCastExpr* cast) const {
        return cast->getSubExpr()->getType() == cast->getType() || 
               cast->getCastKind() == CK_LValueToRValue;
    }

    ASTContext& context;
    TypeConversionTracker& tracker;
    std::string currentFunction;
};

class ImplicitCastConsumer : public ASTConsumer {
public:
    explicit ImplicitCastConsumer(CompilerInstance& ci) 
        : tracker(), visitor(ci.getASTContext(), tracker) {}

    void HandleTranslationUnit(ASTContext& context) override {
        visitor.TraverseDecl(context.getTranslationUnitDecl());
        tracker.printResults(llvm::outs());
    }

private:
    TypeConversionTracker tracker;
    ImplicitCastVisitor visitor;
};

class ImplicitCastAction : public PluginASTAction {
public:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& ci, 
                                                 llvm::StringRef file) override {
        return std::make_unique<ImplicitCastConsumer>(ci);
    }

    bool ParseArgs(const CompilerInstance& ci, 
                  const std::vector<std::string>& args) override {
        return true;
    }

    ActionType getActionType() override {
        return AddBeforeMainAction;
    }
};

static FrontendPluginRegistry::Add<ImplicitCastAction>
    X("implicit-cast-counter", "Count implicit type casts in code");