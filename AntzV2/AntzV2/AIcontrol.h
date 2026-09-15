
#pragma once
// AIcontrol  Win32 AntzV2 Game
#include <Windows.h>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include "GameTypes.h"
#include "Field.h"


class AIcontrol
{
private:
    int m_difficulty = 1;
    std::vector<float> m_attackRatios;
    float m_attackRatioMedian = 0.0f;
    float m_attackRatioMin = 0.0f;
    float m_attackRatioMax = 0.0f;

    struct ReachableCell
    {
        int row;
        int col;
        int distance;
    };
    struct Candidate
    {
        int row;
        int col;
        int threat = 0;
        int required = 0;
    };
    std::vector<Candidate> FindCandidates(
        Field& field,
        std::function<bool(int, int)> rule)
    {
        std::vector<Candidate> candidates;

        for (int row = 0; row < field.getRows(); row++)
        {
            for (int col = 0; col < field.getCols(); col++)
            {
                if (rule(row, col))
                {
                    candidates.push_back({ row, col });
                }
            }
        }

        return candidates;
    }
    std::vector<Candidate> FindAdjacentCandidates(
        Field& field,
        int srow,
        int scol,
        std::function<bool(int, int)> rule)
    {
        std::vector<Candidate> candidates;

        Field::Area area = field.getArea(srow, scol);

        for (int row = area.rmin; row <= area.rmax; row++)
        {
            for (int col = area.cmin; col <= area.cmax; col++)
            {
                if (row == srow && col == scol)
                    continue;

                if (rule(row, col))
                    candidates.push_back({ row, col });
            }
        }

        return candidates;
    }
    std::vector<Candidate> Filter(
        const std::vector<Candidate>& candidates,
        std::function<bool(int, int)> rule)
    {
        std::vector<Candidate> filtered;

        for (const Candidate& candidate : candidates)
        {
            if (rule(candidate.row, candidate.col))
            {
                filtered.push_back(candidate);
            }
        }

        return filtered;
    }
    std::vector<Candidate> Filter(
        const std::vector<Candidate>& candidates,
        std::function<bool(const Candidate&)> rule)
    {
        std::vector<Candidate> filtered;

        for (const Candidate& candidate : candidates)
        {
            if (rule(candidate))
            {
                filtered.push_back(candidate);
            }
        }

        return filtered;
    }
    std::vector<Candidate> Best(const std::vector<Candidate>& candidates,
        std::function<int(int, int)> score)
    {
        std::vector<Candidate> bestCandidates;

        int bestValue = -1;

        for (const Candidate& cell : candidates)
        {
            int value = score(cell.row, cell.col);

            if (value > bestValue)
            {
                bestValue = value;
                bestCandidates.clear();
                bestCandidates.push_back(cell);
            }
            else if (value == bestValue)
            {
                bestCandidates.push_back(cell);
            }
        }

        return bestCandidates;
    }

    int GetThreat(Field& field, int m_playerID, const Candidate& candidate)
    {
        int threat = 0;

        auto adjacent = FindAdjacentCandidates(
            field, candidate.row, candidate.col, [&](int row, int col)
            {
                int owner = field.getOwner(row, col);

                return owner != m_playerID;
            });

        for (const Candidate& cell : adjacent)
        {
            threat = max(threat, field.getAnts(cell.row, cell.col));
        }

        return threat;
    }

    std::vector<ReachableCell> FindReachableTargets(
        Field& field,
        int m_playerID,
        int srow,
        int scol)
    {

        std::vector<ReachableCell> visited;
        std::vector<ReachableCell> open;

        // First flood: find all friendly cells connected to the source.
        open.push_back({ srow, scol, -1 });
        visited.push_back({ srow, scol, -1 });

        size_t index = 0;

        while (index < open.size())
        {
            ReachableCell current = open[index++];

            auto adjacent = FindAdjacentCandidates(
                field,
                current.row,
                current.col,
                [&](int row, int col)
                {
                    return field.getOwner(row, col) == m_playerID;
                });

            for (const Candidate& cell : adjacent)
            {
                bool alreadyVisited = false;

                for (const ReachableCell& visitedCell : visited)
                {
                    if (visitedCell.row == cell.row &&
                        visitedCell.col == cell.col)
                    {
                        alreadyVisited = true;
                        break;
                    }
                }

                if (alreadyVisited)
                    continue;

                visited.push_back({ cell.row, cell.col, -1 });
                open.push_back({ cell.row, cell.col, -1 });
            }
        }
        // Connected front-line cells are distance 0.
        open.clear();

        for (ReachableCell& cell : visited)
        {
            if (field.getStr(cell.row, cell.col) < 8)
            {
                cell.distance = 0;
                open.push_back(cell);
            }
        }

        // No connected front line.
        if (open.empty())
            return {};

        // Second flood: calculate distance from every connected
    // cell to its nearest connected front-line cell.
        index = 0;

        while (index < open.size())
        {
            ReachableCell current = open[index++];

            auto adjacent = FindAdjacentCandidates(
                field,
                current.row,
                current.col,
                [&](int row, int col)
                {
                    return field.getOwner(row, col) == m_playerID;
                });

            for (const Candidate& cell : adjacent)
            {
                for (ReachableCell& visitedCell : visited)
                {
                    if (visitedCell.row == cell.row &&
                        visitedCell.col == cell.col)
                    {
                        if (visitedCell.distance == -1)
                        {
                            visitedCell.distance =
                                current.distance + 1;

                            open.push_back(visitedCell);
                        }

                        break;
                    }
                }
            }
        }

        return visited;
    }
    struct Decision
    {
        int srow = -1;
        int scol = -1;
        int trow = -1;
        int tcol = -1;
        int sant = 0;
        int tant = 0;
    };

    Decision m_decision;
    Decision m_rawDecision;
    std::vector<Decision> m_decisions;
	std::vector<std::vector<Battlefield>> m_battlefield; // Raw battlefield data
    const std::vector<PlayerData>& m_players;

    void AntSelect(Field& field, const std::vector<Candidate>& growth, int availableAnts)
    {

        std::vector<Candidate> ranked = growth;

        std::sort(ranked.begin(), ranked.end(),
            [&](const Candidate& a, const Candidate& b)
            {
                if (a.required != b.required)
                    return a.required < b.required;

                int aAnts = field.getAnts(a.row, a.col);
                int bAnts = field.getAnts(b.row, b.col);

                if (aAnts != bAnts)
                    return aAnts > bAnts;

                return false;
            });

        std::vector<Candidate> funded;

        int remaining = availableAnts;

        for (const Candidate& candidate : ranked)
        {
            if (candidate.required > remaining)
                break;

            funded.push_back(candidate);
            remaining -= candidate.required;
        }
        std::vector<int> allocation;

        if (funded.empty())
        {
            funded.push_back(ranked.front());
            allocation.push_back(availableAnts);
        }
        else
        {
            for (const Candidate& candidate : funded)
            {
                allocation.push_back(candidate.required);
            }

            while (remaining > 0)
            {
                std::vector<int> extraCandidates;

                for (int i = 0; i < static_cast<int>(funded.size()); i++)
                {
                    if (funded[i].required > 1)
                        extraCandidates.push_back(i);
                }

                if (extraCandidates.empty())
                {
                    for (int i = 0; i < static_cast<int>(funded.size()); i++)
                    { extraCandidates.push_back(i); }
                }

                std::uniform_int_distribution<int> dist(
                    0,
                    static_cast<int>(extraCandidates.size()) - 1);

                allocation[extraCandidates[dist(gRNG)]]++;
                remaining--;
            }
        }

        for (int i = 0; i < static_cast<int>(funded.size()); i++)
        {
            Decision decision;

            decision.srow = funded[i].row;
            decision.scol = funded[i].col;
            decision.trow = funded[i].row;
            decision.tcol = funded[i].col;
            decision.tant = allocation[i];

            m_decisions.push_back(decision);
        }
    }
    std::vector<Candidate> AggressiveGrowth(
        Field& field,
        int m_playerID,
        const std::vector<Candidate>& candidates)
    {
        return Filter(candidates, [&](int row, int col)
            {
                auto adjacent = FindAdjacentCandidates(
                    field, row, col,
                    [&](int r, int c)
                    {
                        int owner = field.getOwner(r, c);

                        return owner != 0 && owner != m_playerID;
                    });

                return !adjacent.empty();
            });
    }
    std::vector<Candidate> DefensiveGrowth(
        Field& field,
        int m_playerID,
        const std::vector<Candidate>& candidates)
    {
        auto threatened = Filter(candidates, [&](const Candidate& candidate)
            {
                return candidate.threat > 0;
            });

        if (!threatened.empty())
        {
            return threatened;
        }

        return Filter(candidates, [&](int row, int col)
            {
                auto adjacent = FindAdjacentCandidates(
                    field, row, col,
                    [&](int r, int c)
                    {
                        return field.getOwner(r, c) == 0;
                    });

                return !adjacent.empty();
            });
    }

    int GetTempTStr( Field& field, int m_playerID, int row, int col)
    {
        int tempTstr = 0;

        auto adjacent = FindAdjacentCandidates(
            field, row, col,
            [&](int r, int c)
            {
                return true;
            });

        for (const Candidate& cell : adjacent)
        {
            if (field.getOwner(cell.row, cell.col) == m_playerID)
            {
                tempTstr++;
            }
        }

        return tempTstr;
    }

    void ScoreTargets(const std::vector<PlayerData>& players, int m_playerID,
        const std::vector<Candidate>& sources)
    {
        wchar_t header[256];

        swprintf_s(header, 256,
            L"Turn  Source  SAnts  Target  TAnts  Ratio  TStr  Score\n");

        OutputDebugStringW(header);

        for (const Candidate& source : sources)
        {
            Battlefield& sourceCell =
                m_battlefield[source.row][source.col];

            for (Battlefield::ExCandidate& attack : sourceCell.attacks)
            {
                Battlefield& targetCell =
                    m_battlefield[attack.Row][attack.Col];
                float decimalScore = 0.5f;

                // Score tempTstr according to persona.
                if (targetCell.tOwner == 0)
                {
                    if (players[m_playerID].m_persona.attack == Aggressive)
                    {
                        decimalScore = 0.2f;
                    }
                    else if (players[m_playerID].m_persona.attack == Defensive)
                    {
                        decimalScore = 0.8f;
                    }
                }
                else {
                    // Score the attackRatio from the global median.
                    if (attack.attackRatio < m_attackRatioMedian)
                    {
                        decimalScore = 0.5f - ((m_attackRatioMedian - attack.attackRatio) /
                            (m_attackRatioMedian - m_attackRatioMin)) * 0.4f;
                    }
                    else if (attack.attackRatio > m_attackRatioMedian)
                    {
                        decimalScore = 0.5f + ((attack.attackRatio - m_attackRatioMedian) /
                            (m_attackRatioMax - m_attackRatioMedian)) * 0.4f;
                    }
                    else
                    {
                        decimalScore = 0.5f;
                    }
                }

                // Score tempTstr according to persona.
                int integerScore = attack.tempTstr;

                attack.tempScore =
                    static_cast<float>(integerScore) + decimalScore;

                //Debug data log
                wchar_t data[256];

                swprintf_s(data, 256,
                    L"%4d  %4d,%d  %5d  %4d,%d  %5d  %5.2f  %4d  %5.1f\n",
                    gTurn, source.row, source.col, sourceCell.sAnts,
                    attack.Row, attack.Col, attack.Ants,
                    attack.attackRatio, attack.tempTstr,
                    attack.tempScore);

                OutputDebugStringW(data);
            }
        }
    }

    void RawField( Field& field, int m_playerID, const std::vector<Candidate>& sources)
    {
        m_battlefield.clear();
        m_attackRatios.clear();
        m_attackRatioMedian = 0.0f;

        m_battlefield.resize( field.getRows(), std::vector<Battlefield>(field.getCols()));

        for (const Candidate& source : sources)
        {
            Battlefield& sourceCell = m_battlefield[source.row][source.col];

            sourceCell.sRow = source.row;
            sourceCell.sCol = source.col;
            sourceCell.sAnts = field.getAnts(source.row, source.col);
            sourceCell.sBase = field.getBase(source.row, source.col);
            sourceCell.sStr = field.getStr(source.row, source.col);

            auto adjacent = FindAdjacentCandidates(
                field, source.row, source.col, [&](int row, int col)
                {
                    return true;
                });

            for (const Candidate& cell : adjacent)
            {
                if (field.getOwner(cell.row, cell.col) == m_playerID)
                {
                    Battlefield& friendlyCell = m_battlefield[cell.row][cell.col];

                    friendlyCell.fRow = cell.row;
                    friendlyCell.fCol = cell.col;
                    friendlyCell.fAnts = field.getAnts(cell.row, cell.col);
                    friendlyCell.fBase = field.getBase(cell.row, cell.col);
                    friendlyCell.fStr = field.getStr(cell.row, cell.col);

                }
                else
                {
                    Battlefield& targetCell =
                        m_battlefield[cell.row][cell.col];

                    targetCell.tOwner = field.getOwner(cell.row, cell.col);
                    targetCell.tRow = cell.row;
                    targetCell.tCol = cell.col;
                    targetCell.tAnts = field.getAnts(cell.row, cell.col);
                    targetCell.tBase = field.getBase(cell.row, cell.col);

                    int availableAnts = sourceCell.sAnts - 1;
                    if (targetCell.tOwner != 0)
                    {
                        // Enemy-owned target.
                        
                        float attackRatio =
                            static_cast<float>(availableAnts) / targetCell.tAnts;

                        if (attackRatio >= 1.35f)
                        {
                            Battlefield::ExCandidate attack;

                            attack.Row = targetCell.tRow;
                            attack.Col = targetCell.tCol;
                            attack.Ants = targetCell.tAnts;
                            attack.attackRatio = attackRatio;
                            m_attackRatios.push_back(attack.attackRatio);

                            attack.tempTstr = GetTempTStr( field, m_playerID, 
                                targetCell.tRow, targetCell.tCol);

                            sourceCell.attacks.push_back(attack);

                            Battlefield::ExCandidate sourceCandidate;

                            sourceCandidate.Row = source.row;
                            sourceCandidate.Col = source.col;
                            sourceCandidate.Ants = sourceCell.sAnts;

                            targetCell.sources.push_back(sourceCandidate);

                        }
                    }
                    else
                    {
                        Battlefield::ExCandidate attack;

                        attack.Row = targetCell.tRow;
                        attack.Col = targetCell.tCol;
                        attack.Ants = targetCell.tAnts;

                        attack.tempTstr = GetTempTStr(
                            field,
                            m_playerID,
                            targetCell.tRow,
                            targetCell.tCol);

                        sourceCell.attacks.push_back(attack);

                        Battlefield::ExCandidate sourceCandidate;

                        sourceCandidate.Row = source.row;
                        sourceCandidate.Col = source.col;
                        sourceCandidate.Ants = sourceCell.sAnts;

                        targetCell.sources.push_back(sourceCandidate);
                    }
                }
            }
        }
        if (!m_attackRatios.empty())
        {
            auto ratios = m_attackRatios;

            std::sort(ratios.begin(), ratios.end());

            m_attackRatioMin = ratios.front();
            m_attackRatioMax = ratios.back();

            int count = static_cast<int>(ratios.size());

            if (count % 2 == 0)
            {
                m_attackRatioMedian =
                    (ratios[count / 2 - 1] + ratios[count / 2]) / 2.0f;
            }
            else
            {
                m_attackRatioMedian =
                    ratios[count / 2];
            }
        }
    }
    void RawDecision(int m_playerID)
    {
        m_rawDecision = {};
        struct RawCandidate
        {
            int srow;
            int scol;
            int trow;
            int tcol;
            int strategic;
            int tactical;
        };

        std::vector<RawCandidate> candidates;

        for (const auto& row : m_battlefield)
        {
            for (const auto& cell : row)
            {
                for (const Battlefield::ExCandidate& attack : cell.attacks)
                {
                    int strategic = static_cast<int>(attack.tempScore);

                    int tactical =
                        static_cast<int>(
                            (attack.tempScore - strategic) * 10.0f + 0.5f);

                    candidates.push_back(
                        {
                            cell.sRow,
                            cell.sCol,
                            attack.Row,
                            attack.Col,
                            strategic,
                            tactical
                        });
                }
            }
        }

        if (candidates.empty())
            return;

        std::vector<RawCandidate> previous = candidates;

        if (m_players[m_playerID].m_persona.attack == Aggressive)
        {
            std::vector<RawCandidate> filtered;

            for (const RawCandidate& candidate : candidates)
            {
                if (candidate.tactical > 5)
                    filtered.push_back(candidate);
            }

            if (!filtered.empty())
                candidates = filtered;
            else
                candidates = previous;

            std::sort(candidates.begin(), candidates.end(),
                [](const RawCandidate& a, const RawCandidate& b)
                {
                    if (a.tactical != b.tactical)
                        return a.tactical > b.tactical;

                    return a.strategic > b.strategic;
                });
        }
        else if (m_players[m_playerID].m_persona.attack == Defensive)
        {
            std::vector<RawCandidate> filtered;

            for (const RawCandidate& candidate : candidates)
            {
                if (candidate.strategic > 5)
                    filtered.push_back(candidate);
            }

            if (!filtered.empty())
                candidates = filtered;
            else
                candidates = previous;

            std::sort(candidates.begin(), candidates.end(),
                [](const RawCandidate& a, const RawCandidate& b)
                {
                    if (a.strategic != b.strategic)
                        return a.strategic > b.strategic;

                    return a.tactical > b.tactical;
                });
        }
        else    // Balanced
        {
            std::vector<RawCandidate> filtered;

            for (const RawCandidate& candidate : candidates)
            {
                if (candidate.strategic >= 4 &&
                    candidate.tactical >= 5)
                {
                    filtered.push_back(candidate);
                }
            }

            if (!filtered.empty())
                candidates = filtered;
            else
                candidates = previous;
        }

        RawCandidate choice;

        if (m_players[m_playerID].m_persona.attack == Balanced)
        {
            std::uniform_int_distribution<int> dist(
                0,
                static_cast<int>(candidates.size()) - 1);

            choice = candidates[dist(gRNG)];
        }
        else
        {
            std::vector<RawCandidate> best;

            best.push_back(candidates.front());

            for (size_t i = 1; i < candidates.size(); i++)
            {
                if (candidates[i].strategic == candidates.front().strategic &&
                    candidates[i].tactical == candidates.front().tactical)
                {
                    best.push_back(candidates[i]);
                }
                else
                {
                    break;
                }
            }

            std::uniform_int_distribution<int> dist(
                0,
                static_cast<int>(best.size()) - 1);

            choice = best[dist(gRNG)];
        }

        wchar_t data[256];

        swprintf_s(data, 256,
            L"Decision: %d,%d -> %d,%d\n",
            choice.srow,
            choice.scol,
            choice.trow,
            choice.tcol);

        OutputDebugStringW(data);

        m_rawDecision.srow = choice.srow;
        m_rawDecision.scol = choice.scol;
        m_rawDecision.trow = choice.trow;
        m_rawDecision.tcol = choice.tcol;
    }


public:
    AIcontrol(const std::vector<PlayerData>& players) : m_players(players)
    {
    }

    void Growth(Field& field, int m_playerID, int availableAnts)
    {
        m_decision = {};

        if (m_decisions.empty())
        {
            auto candidates = FindCandidates(field,
                [&](int row, int col)
                {
                    if (field.getOwner(row, col) != m_playerID)
                        return false;

                    return field.getStr(row, col) < 8;
                });

            // No valid growth target
            if (candidates.empty())
            {
                return;
            }
            auto personaCandidates = candidates;

            if (m_players[m_playerID].m_persona.growth == Aggressive)
            {
                personaCandidates = AggressiveGrowth(
                    field, m_playerID, candidates);
            }
            else if (m_players[m_playerID].m_persona.growth == Defensive && m_difficulty == 1)
            {
                personaCandidates = DefensiveGrowth(
                    field, m_playerID, candidates);
            }

            if (!personaCandidates.empty())
            {
                candidates = personaCandidates;
            }

            if (m_difficulty > 1)
            {
                auto growth = Filter(candidates, [&](int row, int col)
                    {
                        return field.getStr(row, col) >= 4;
                    });

                if (growth.empty())
                {
                    growth = candidates;
                }

                for (Candidate& candidate : growth)
                {
                    candidate.threat = GetThreat(field, m_playerID, candidate);

                    if (candidate.threat == 0) { candidate.required = 1; }
                    else
                    {
                        int ants = field.getAnts(candidate.row, candidate.col);

                        int required = ((candidate.threat * 5 + 3) / 4) - ants + 1;

                        candidate.required = max(0, required);
                    }
                }
                if (m_players[m_playerID].m_persona.growth == Defensive)
                {
                    auto defensiveCandidates = DefensiveGrowth(
                        field, m_playerID, growth);

                    if (!defensiveCandidates.empty())
                    {
                        growth = defensiveCandidates;
                    }
                }
                auto viable = Filter(growth, [&](const Candidate& candidate)
                    {
                        return candidate.required > 0;
                    });

                if (viable.empty())
                {
                    // No Medium candidate requires growth.
                    AntSelect(field, growth, availableAnts);
                }
                else
                {
                    AntSelect(field, viable, availableAnts);
                }
            }
            else
            {
                auto bestCandidates = Best(candidates, [&](int row, int col)
                    {
                        return field.getStr(row, col);
                    });

                std::uniform_int_distribution<int> dist(0,
                    static_cast<int>(bestCandidates.size()) - 1);

                const Candidate& target =
                    bestCandidates[dist(gRNG)];

                m_decision.srow = target.row;
                m_decision.scol = target.col;
                m_decision.trow = target.row;
                m_decision.tcol = target.col;
                m_decision.tant = availableAnts;
                wchar_t data[256];
                swprintf_s(data, 256,
                    L"Growth:  Player %d -> %d,%d with %d ants\n",
                    m_playerID,
                    m_decision.srow,
                    m_decision.scol,
                    m_decision.tant);
                OutputDebugStringW(data);
            }
        }

        if (!m_decisions.empty())
        {
            m_decision = m_decisions.back();
            wchar_t data[256];
            swprintf_s(data, 256,
                L"Growth:  Player %d -> %d,%d with %d ants\n",
                m_playerID,
                m_decision.srow,
                m_decision.scol,
                m_decision.tant);
            OutputDebugStringW(data);

            m_decisions.pop_back();

            return;
        }
    }

    void Attack(Field & field, int m_playerID)
        {

            auto candidates = FindAdjacentCandidates(
                field,
                m_decision.srow,
                m_decision.scol,
                [&](int row, int col)
                {
                    return field.getOwner(row, col) != m_playerID;
                });

            if (candidates.empty())
            {
                return;
            }

            std::uniform_int_distribution<int> dist(
                0,
                static_cast<int>(candidates.size()) - 1);

            const Candidate& target = candidates[dist(gRNG)];

            m_decision.trow = target.row;
            m_decision.tcol = target.col;

            int available =
                field.getAnts(m_decision.srow, m_decision.scol) - 1;
            if (available <= 0)
            {
                return;
            }
            m_decision.sant = available;
            wchar_t data[256];
            swprintf_s(data, 256,
                L"Attack: %d,%d -> %d,%d with %d ants\n",
                m_decision.srow,
                m_decision.scol,
                m_decision.trow,
                m_decision.tcol,
				m_decision.sant);
            OutputDebugStringW(data);

    }

    void Move(Field& field, int m_playerID)
    {
        m_decision = {};

        // Find possible source cells
        auto sources = FindCandidates(field, [&](int row, int col)
            {
                if (field.getOwner(row, col) != m_playerID)
                    return false;

                // Movement source must be a base
                if (field.getStr(row, col) != 8)
                    return false;

                // Need ants available to move
                if (field.getAnts(row, col) <= 1)
                    return false;

                return true;
            });

        if (sources.empty())
        {
            // Skip
            m_decision.srow = -1;
            m_decision.scol = -1;
            m_decision.sant = 0;
            return;
        }

        // Pick source with most ants
        auto bestSources = Best(sources,
            [&](int row, int col)
            {
                return field.getAnts(row, col);
            });

        if (bestSources.empty())
        {
            // Skip
            m_decision.srow = -1;
            m_decision.scol = -1;
            m_decision.sant = 0;
            return;
        }

        std::uniform_int_distribution<int> sourceDist(
            0,
            static_cast<int>(bestSources.size()) - 1);

        const Candidate& source = bestSources[sourceDist(gRNG)];

        m_decision.srow = source.row;
        m_decision.scol = source.col;

        // Available ants to move
        m_decision.sant =
            field.getAnts(source.row, source.col) - 1;
        // TARGET:
            // Find all reachable friendly territory from the source.
        auto reachable = FindReachableTargets(
            field,
            m_playerID,
            m_decision.srow,
            m_decision.scol);

        if (reachable.empty())
        {
            // Skip
            m_decision.srow = -1;
            m_decision.scol = -1;
            m_decision.sant = 0;
            return;
        }

        // Find the cells we can actually move to this turn.
        auto adjacent = FindAdjacentCandidates(
            field,
            m_decision.srow,
            m_decision.scol,
            [&](int row, int col)
            {
                return field.getOwner(row, col) == m_playerID;
            });

        if (adjacent.empty())
        {
            // Skip
            m_decision.srow = -1;
            m_decision.scol = -1;
            m_decision.sant = 0;
            return;
        }

        // Keep only adjacent cells that lead into the reachable territory.
        auto targets = Filter(
            adjacent,
            [&](int row, int col)
            {
                for (const ReachableCell& cell : reachable)
                {
                    if (cell.row == row && cell.col == col)
                    {
                        return true;
                    }
                }
                return false;
            });

        if (targets.empty())
        {
            // Skip
            m_decision.srow = -1;
            m_decision.scol = -1;
            m_decision.sant = 0;
            return;
        }

        // Apply move persona
        if (m_players[m_playerID].m_persona.move == Aggressive)
        {
            auto bestPersonaTargets = Best(targets,
                [&](int row, int col)
                {
                    return field.getAnts(row, col);
                });

            targets = bestPersonaTargets;
        }
        else if (m_players[m_playerID].m_persona.move == Defensive)
        {
            auto bestPersonaTargets = Best(targets,
                [&](int row, int col)
                {
                    return -field.getAnts(row, col);
                });

            targets = bestPersonaTargets;
        }

        // Pick target.
        auto bestTargets = Best(targets,
            [&](int row, int col)
            {
                for (const ReachableCell& cell : reachable)
                {
                    if (cell.row == row && cell.col == col)
                        return -cell.distance;
                }

                return -1;
            });

        if (bestTargets.empty())
        {
            // Skip
            m_decision.srow = -1;
            m_decision.scol = -1;
            m_decision.sant = 0;
            return;
        }

        std::uniform_int_distribution<int> targetDist(
            0,
            static_cast<int>(bestTargets.size()) - 1);

        const Candidate& target = bestTargets[targetDist(gRNG)];

        m_decision.trow = target.row;
        m_decision.tcol = target.col;

        // Move all but the defending ant
        m_decision.tant = m_decision.sant;
        wchar_t data[256];
        swprintf_s(data, 256,
            L"Move:  Player %d   %d,%d -> %d,%d with %d ants\n",
            m_playerID,
            m_decision.srow,
            m_decision.scol,
            m_decision.trow,
            m_decision.tcol,
            m_decision.tant);
        OutputDebugStringW(data);
    }

    void ExAttack(Field& field, int m_playerID)
    {
        m_decision = {};

        // 1. Find possible source cells
        auto sources = FindCandidates(field, [&](int row, int col)
            {
                if (field.getOwner(row, col) != m_playerID)
                    return false;
                if (field.getStr(row, col) == 8)
                    return false;
                if (field.getAnts(row, col) <= 1)
                    return false;
                return true;
            });

        if (sources.empty())
		{   // Forced Skip: No valid source cells
            return;
        }
        if (m_players[m_playerID].m_persona.attack == Aggressive)
        {
            auto filtered = Filter(sources, [&](int row, int col)
                {
                    auto adjacent = FindAdjacentCandidates(
                        field, row, col, [&](int r, int c)
                        {
                            int owner = field.getOwner(r, c);

                            return owner != 0 && owner != m_playerID;
                        });

                    return !adjacent.empty();
                });

            if (!filtered.empty())
                sources = filtered;
        }
        else if (m_players[m_playerID].m_persona.attack == Defensive)
        {
            auto filtered = Filter(sources, [&](int row, int col)
                {
                    auto adjacent = FindAdjacentCandidates(
                        field, row, col, [&](int r, int c)
                        {
                            return field.getOwner(r, c) == 0;
                        });

                    return !adjacent.empty();
                });

            if (!filtered.empty())
                sources = filtered;
        }
        if (sources.empty())
        {   // Forced Skip: No valid source cells
            return;
        }

        //Hard difficulty expansion

        if (m_difficulty > 2)
        {

        RawField(field, m_playerID, sources);
        ScoreTargets(m_players, m_playerID, sources); // scoring
        RawDecision(m_playerID);                     // Decision

        if (m_rawDecision.srow < 0)  //Forced skip
            return;

        m_decision.srow = m_rawDecision.srow;
        m_decision.scol = m_rawDecision.scol;
        m_decision.sant =
            field.getAnts(m_decision.srow, m_decision.scol) - 1;
        m_decision.trow = m_rawDecision.trow;
        m_decision.tcol = m_rawDecision.tcol;

        wchar_t data[256];
        swprintf_s(data, 256,
            L"ExAttack: %d,%d -> %d,%d with %d ants\n",
            m_decision.srow,
            m_decision.scol,
            m_decision.trow,
            m_decision.tcol,
            m_decision.sant);
        OutputDebugStringW(data);

        return;
        }

        auto bestCandidates = Best( sources, [&](int row, int col)
            {
                return field.getAnts(row, col);
            });

        std::uniform_int_distribution<int> sourceDist( 0,
            static_cast<int>(bestCandidates.size()) - 1);

        const Candidate& source = bestCandidates[sourceDist(gRNG)];

        m_decision.srow = source.row;
        m_decision.scol = source.col;
        m_decision.sant = field.getAnts(source.row, source.col) - 1;
        auto targets = FindAdjacentCandidates( field, m_decision.srow,
            m_decision.scol, [&](int row, int col)
            {
                return field.getOwner(row, col) != m_playerID;
            });

        if (targets.empty())
		{   // Forced Skip: No valid target cells
            m_decision.srow = -1;
            m_decision.scol = -1;
            m_decision.sant = 0;
            return;
        }
        if (m_players[m_playerID].m_persona.attack == Aggressive)
        {
            targets = Filter(targets, [&](int row, int col)
                {
                    double ratio =
                        static_cast<double>(m_decision.sant) /
                        field.getAnts(row, col);

                    return ratio >= 1.2;
                });
        }
        else if (m_players[m_playerID].m_persona.attack == Defensive)
        {
            targets = Filter(targets, [&](int row, int col)
                {
                    double ratio =
                        static_cast<double>(m_decision.sant) /
                        field.getAnts(row, col);

                    return ratio >= 2.5;
                });
        }
        if (targets.empty())
        {   // Forced Skip: No valid target cells
            m_decision.srow = -1;
            m_decision.scol = -1;
            m_decision.sant = 0;
            return;
        }

            bestCandidates = targets;

            std::uniform_int_distribution<int> targetDist(0,
                static_cast<int>(bestCandidates.size()) - 1);

            const Candidate& target = bestCandidates[targetDist(gRNG)];

            m_decision.trow = target.row;
            m_decision.tcol = target.col;

            wchar_t data[256];
            swprintf_s(data, 256,
                L"ExAttack: %d,%d -> %d,%d with %d ants\n",
                m_decision.srow,
                m_decision.scol,
                m_decision.trow,
                m_decision.tcol,
                m_decision.sant);
            OutputDebugStringW(data);
        
    }

    // Getters & Setters

    void SetDifficulty(int difficulty) { m_difficulty = difficulty; }

    int GetSourceRow() const
    {
        return m_decision.srow;
    }

    int GetSourceCol() const
    {
        return m_decision.scol;
    }

    int GetTargetRow() const
    {
        return m_decision.trow;
    }

    int GetTargetCol() const
    {
        return m_decision.tcol;
    }

    int GetSourceAnts() const
    {
        return m_decision.sant;
    }

    int GetTargetAnts() const
    {
        return m_decision.tant;
    }
    
    const std::vector<std::vector<Battlefield>>& GetBattlefield() const
    {
        return m_battlefield;
    }

};

