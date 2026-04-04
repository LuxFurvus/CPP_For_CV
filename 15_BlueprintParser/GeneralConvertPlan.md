
# ===GENERAL PLAN===

-----------
## Expeted Data Types
-----------

```cpp

enum class EPinContainerType : uint8
{
    None,
    Array,
    Set,
    Map,
};/////

enum EEdGraphPinDirection : uint8
{
    EGPD_Input,
    EGPD_Output,
    EGPD_MAX,
};/////

enum EK2NodeType : uint8
{
    K2Node_FunctionEntry,
    K2Node_FunctionResult,
    K2Node_PromotableOperator,
    K2Node_CallFunction,
    K2Node_IfThenElse,
    K2Node_MacroInstance_ForEachLoop,
    K2Node_MakeArray,
    K2Node_VariableGet,
    K2Node_Knot,
};/////

struct FNodeGuid
{
    const uint64_t Value;
};/////

struct FPinGuid
{
    const uint64_t Value;
};/////

struct FGraphBuilder
{
    struct FNodeObj
    {
        struct FPin
        {
            FPinGuid PinId{};
            FPinGuid LinkedTo{};
            std::string_view PinName{};
            EDataType PinCategory{};
            EPinContainerType ContainerType{};
            EEdGraphPinDirection Direction{};
        };/////

        std::string_view NodeSubstring{};
        EK2NodeType Class;
        FNodeGuid ThisNodeGuid;
        std::vector<FNodeGuid> ParentNodes;
        std::vector<FNodeGuid> ChildNodes;
        std::unordered_map<FPinGuid, FPin> InputPins{};
        std::unordered_map<FPinGuid, FPin> OutputPins{};
    };/////

    std::shared_ptr<const std::string> GraphString{};
    std::shared_ptr<const FNodeObj> StartNode{};
    std::vector<FNodeObj> AllNodes{};
    NodeDag ExecutionFlow{};
};/////

```

-----------
## STAGE 1: Extract Node Substrings
-----------

0. **Create FGraphBuilder object to store all intermediate info**

```cpp
struct FGraphBuilder;
FGraphBuilder Graph;
EXPECT_TRUE(Graph.GraphString != nullptr);
```

=========================
1. **Single Node Extraction**
    Create a function that takes the full Blueprint text and finds the first substring between
    `Begin Object Class=/Script/BlueprintGraph.` and `End Object`.
    It should return:
        - the extracted node substring,
        - the position of the end marker (to continue parsing the next node).
    Assumptions:
        - Each `Begin` corresponds to one `End`, no nesting.

```cpp

const std::pair<std::string, std::string> NodeMarkers
{
    "Begin Object Class=/Script/BlueprintGraph.",
    "End Object",
};

std::pair<std::string, size_t> GetFirstSubstring(
    const std::string& GraphString,
    const size_t StartPos,
    const std::pair<std::string, std::string>& StartEndMarkers);
```

=========================
2. **Collect All Nodes**
    Create a function that repeatedly calls the first function to gather **all node substrings** into an array.
    This produces a collection of raw nodes ready for further parsing.

    Assumptions:
        - The array of node subtrings need not to preserve the order of nodes in the source text.

```cpp
std::vector<std::string_view> GetAllSubstrings(
    const std::string& GraphString,
    const std::pair<std::string, std::string>& StartEndMarkers);

EXPECT_TRUE(Graph.GraphString != nullptr);
EXPECT_TRUE(Graph.AllNodes.empty() == false);
EXPECT_TRUE(Graph.AllNodes[i].NodeSubstring.empty() == false);
```

-----------
## STAGE 2: Build Execution Flow DAG
-----------

1. Parse all Nodes for `NodeGuid` and `Pins`
    - Extract `NodeGuid`
    - Extract `Class`
    - Extract each `Pin`
    - Если пина содержит `bHidden=True` и явлается `self`, игнорируем ее.
    - Из каждой ноды выделяем: `PinId`, `PinName`, `PinType.PinCategory`
    - Разделяем эти `Pins` into `Input` and `Output` arrays.
    - Remove K2Node_Knot nodes: find its `Input` pin and replace `Output` pin to the Knot with the `Output` pit of the Knot
```cpp
std::unordered_map<uint64_t, FPin> ParseNodePins(
    const std::string_view NodeSubstring);

EXPECT_TRUE(Graph.GraphString != nullptr);
EXPECT_TRUE(Graph.AllNodes.empty() == false);
EXPECT_TRUE(Graph.AllNodes[i].NodeSubstring.empty() == false);
EXPECT_TRUE(Graph.AllNodes[i].InputPins.empty() == false);
EXPECT_TRUE(Graph.AllNodes[i].OutputPins.empty() == false);
EXPECT_TRUE(Graph.AllNodes[i].Class != K2Node_Knot);
```

2. For each node fill `ParentNodes` and `ChildNodes` from `Pins`

```cpp
std::pair<std::vector<uint64_t>,std::vector<uint64_t>> GetParentAndChildNodeGuids(
    const std::vector<FNodeObj>& AllNodes);

EXPECT_TRUE(Graph.GraphString != nullptr);
EXPECT_TRUE(Graph.AllNodes.empty() == false);
EXPECT_TRUE(Graph.AllNodes[i].NodeSubstring.empty() == false);
EXPECT_TRUE(Graph.AllNodes[i].InputPins.empty() == false);
EXPECT_TRUE(Graph.AllNodes[i].OutputPins.empty() == false);
EXPECT_TRUE(Graph.AllNodes[i].ParentNodes.empty() == false);
EXPECT_TRUE(Graph.AllNodes[i].ChildNodes.empty() == false);
```

2. Строим DAG из `exec-нод`, по одному нод-объекту перемещая из AllNodes в ExecutionFlow
Если мы встречаем ноду с двумя `exec-pin`, мы завершаем построение текущего массива и начинаем два параллельных массива
Если мы встречаем `pure-ноду`, мы создаем массив таких нод и помещаем в него ноды, от пинов которых она зависит, пока не встретим `exec-ноду`. Выражения из этого массива будут размещаеться сразу после выражения этой ноды. Презюмируется, что поток однонаправленный. Потому, если мы встречаем на пути ноду, уже присутствующую в DAG, мы игнорируем эту связь
По итогу этого этапа у нас будет directed acyclic graphs контейнер из `NodeGuid` всех нод
```cpp
NodeDag BuildExecutionFlow(std::vector<FNodeObj>& AllNodes);

EXPECT_TRUE(Graph.GraphString != nullptr);
EXPECT_TRUE(Graph.AllNodes.empty() == true);
EXPECT_TRUE(Graph.ExecutionFlow.IsEmpty() == false);
EXPECT_TRUE(Graph.ExecutionFlow[i].NodeSubstring.empty() == false);
EXPECT_TRUE(Graph.ExecutionFlow[i].InputPins.empty() == false);
EXPECT_TRUE(Graph.ExecutionFlow[i].OutputPins.empty() == false);
EXPECT_TRUE(Graph.ExecutionFlow[i].ParentNodes.empty() == false);
EXPECT_TRUE(Graph.ExecutionFlow[i].ChildNodes.empty() == false);
```

-----------
## STAGE 3: Compose C++ code
-----------

0. В каждый класса ноды заранее прописываем полиморфный класс-фабрику для ее С++ фрагмента кода

```cpp

struct FBuildPair
{
    const std::string_view SlotName;
    const std::function<std::string()> FillingMethod;
};///

std::string FillBlanks(
    const std::string_view Pattern,
    const std::vector<FBuildPair>& BuildPairs)
{
    std::string FilledString{Pattern};

    for (const auto& Pair : BuildPairs)
    {
        const std::string Slot{Pair.SlotName};
        const std::string Value{Pair.FillingMethod()};

        size_t Pos = 0;
        while ((Pos = FilledString.find(Slot, Pos)) != std::string::npos)
        {
            FilledString.replace(Pos, Slot.length(), Value);
            Pos += Value.length();
        }
    }

    return FilledString;
}///

struct K2NodeBase
{

    virtual ~K2NodeBase() = default;

    virtual std::string_view GetPattern() = 0;
    virtual std::vector<FBuildPair> GetBuildPairs() = 0;

    std::string BuildNodeCode()
    {
        return FillBlanks(GetPattern(), GetBuildPairs());
    }
};///

struct K2Node_FunctionEntry : public K2NodeBase
{
    std::string_view GetPattern() override
    {
        return "auto {FunName}({Params})\n{\n{FuncBody}\n}\n";
    }

    std::vector<FBuildPair> GetBuildPairs()
    {
        return
        {
            {"{FunName}", []{return GetFunName();}},
            {"{Params}", []{return GetParams();}},
            {"{FuncBody}", []{return GetFuncBody();}},
        };
    };
};///

struct K2Node_CallFunction : public K2NodeBase
{
    static inline constexpr std::string_view Pattern
        = "{RetType} {RetName} = {FunName}({Args});\n";

    std::vector<FBuildPair> BuildPairs
    {
        {"{RetType}", []{return GetRetType();}},
        {"{RetName}", []{return GetRetName();}},
        {"{FunName}", []{return GetFunName();}},
        {"{Args}", []{return GetArgs();}},
    };
};///

```
1. Обходим каждую ноду в DAG-контейнере:
    - Начинаем в самой верхней ноды
    - Extract Class-specific data
    - Некоторые ноды, которые предполагают собственный scope, буду сами находить в подчиненные ноды
    - Если нода имеет return, т.е. предполагает создание переменной, заносим ее имя в *стек-реестр*
    - Для каждой ноды создаем макет-подстроку, помещаем туда данные, которые у нас есть в самой нод-объекте или в стек-реестре переменных
