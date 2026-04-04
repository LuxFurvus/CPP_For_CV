
#pragma once


#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <queue>
#include <stack>
#include <stdexcept>
#include <functional>

struct FNodeObj
{
};

class NodeDag
{
private:

    struct NodeRecord
    {
        std::optional<FNodeObj> Payload;
    };

    std::unordered_map<std::uint64_t, NodeRecord> Data;
    std::unordered_map<std::uint64_t, std::unordered_set<std::uint64_t>> Adj;

    void EnsureNodeExists(const std::uint64_t Id)
    {
        if (Data.find(Id) != Data.end()) return;
        Data.emplace(Id, NodeRecord{std::optional<FNodeObj>{}});
        Adj.emplace(Id, std::unordered_set<std::uint64_t>{});
    }

    bool HasEdge(const std::uint64_t From, const std::uint64_t To) const
    {
        const auto It = Adj.find(From);
        if (It == Adj.end()) return false;
        return It->second.find(To) != It->second.end();
    }

    bool PathExists(const std::uint64_t FromSource, const std::uint64_t Target) const
    {
        if (FromSource == Target) return true;
        std::stack<std::uint64_t> St;
        std::unordered_set<std::uint64_t> Visited;
        St.push(FromSource);
        while (!St.empty())
        {
            const std::uint64_t V = St.top();
            St.pop();
            if (!Visited.insert(V).second) continue;
            const auto It = Adj.find(V);
            if (It == Adj.end()) continue;
            for (const std::uint64_t To : It->second)
            {
                if (To == Target) return true;
                if (Visited.find(To) == Visited.end()) St.push(To);
            }
        }
        return false;
    }

protected:
public:
    bool AddNode(
        const std::uint64_t NodeGuid,
        FNodeObj ObjToAdd,
        const std::vector<std::uint64_t>& ParentNodeGuids,
        const std::vector<std::uint64_t>& ChildNodeGuids)
    {
        EnsureNodeExists(NodeGuid);
        NodeRecord& Rec = Data.at(NodeGuid);
        if (!Rec.Payload.has_value()) Rec.Payload.emplace(std::move(ObjToAdd));

        for (const std::uint64_t P : ParentNodeGuids) EnsureNodeExists(P);
        for (const std::uint64_t C : ChildNodeGuids) EnsureNodeExists(C);

        for (const std::uint64_t P : ParentNodeGuids)
        {
            if (P == NodeGuid) continue;
            if (HasEdge(P, NodeGuid)) continue;
            if (PathExists(NodeGuid, P)) continue;
            Adj[P].insert(NodeGuid);
        }

        for (const std::uint64_t C : ChildNodeGuids)
        {
            if (C == NodeGuid) continue;
            if (HasEdge(NodeGuid, C)) continue;
            if (PathExists(C, NodeGuid)) continue;
            Adj[NodeGuid].insert(C);
        }

        return true;
    }

    bool Contains(const std::uint64_t Id) const
    {
        const auto It = Data.find(Id);
        if (It == Data.end()) return false;
        return It->second.Payload.has_value();
    }

    const FNodeObj& Get(const std::uint64_t Id) const
    {
        const NodeRecord& Rec = Data.at(Id);
        if (!Rec.Payload.has_value()) throw std::logic_error("Payload not set");
        return Rec.Payload.value();
    }

    const std::unordered_set<std::uint64_t>& GetChildrenId(const std::uint64_t Id) const
    {
        return Adj.at(Id);
    }

    std::size_t Num() const
    {
        std::size_t Count = 0;
        for (const auto& P : Data)
        {
            if (P.second.Payload.has_value()) ++Count;
        }
        return Count;
    }

    bool IsEmpty() const
    {
        for (const auto& P : Data)
        {
            if (P.second.Payload.has_value()) return false;
        }
        return true;
    }

    std::vector<std::uint64_t> TopologicalOrder() const
    {
        std::unordered_map<std::uint64_t, int> Deg;
        for (const auto& P : Data) if (P.second.Payload.has_value()) Deg.emplace(P.first, 0);
        for (const auto& P : Data)
        {
            if (!P.second.Payload.has_value()) continue;
            const auto It = Adj.find(P.first);
            if (It == Adj.end()) continue;
            for (const std::uint64_t To : It->second)
            {
                const auto Dt = Deg.find(To);
                if (Dt != Deg.end()) ++(Dt->second);
            }
        }

        struct Greater { bool operator()(const std::uint64_t A, const std::uint64_t B) const { return A > B; } };
        std::priority_queue<std::uint64_t, std::vector<std::uint64_t>, Greater> Ready;
        for (const auto& P : Deg) if (P.second == 0) Ready.push(P.first);

        std::vector<std::uint64_t> Order;
        Order.reserve(Deg.size());

        while (!Ready.empty())
        {
            const std::uint64_t V = Ready.top();
            Ready.pop();
            Order.push_back(V);

            const auto It = Adj.find(V);
            if (It == Adj.end()) continue;
            for (const std::uint64_t To : It->second)
            {
                const auto Dt = Deg.find(To);
                if (Dt == Deg.end()) continue;
                --(Dt->second);
                if (Dt->second == 0) Ready.push(To);
            }
        }

        if (Order.size() != Deg.size()) throw std::logic_error("Graph contains a cycle");
        return Order;
    }

    void ForEachOnceTopologically(const std::function<void(std::uint64_t, const FNodeObj&)>& Visit) const
    {
        const std::vector<std::uint64_t> Order = TopologicalOrder();
        for (const std::uint64_t Id : Order)
        {
            const auto It = Data.find(Id);
            if (It == Data.end()) continue;
            if (!It->second.Payload.has_value()) continue;
            Visit(Id, It->second.Payload.value());
        }
    }
};


