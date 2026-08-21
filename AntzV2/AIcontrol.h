#pragma once
// AIcontrol  Win32 AntzV2 Game
#include <vector>
#include <functional>
#include <algorithm>
#include "GameTypes.h"
#include "Field.h"

class AIcontrol
{
private:

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
    std::vector<Candidate> Best( const std::vector<Candidate>& candidates,
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

    int AntSelect(int availableAnts)
    {
        // Future hook:
        // Adjust commitment based on difficulty/persona/strategy.

        return availableAnts;
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

    const std::vector<PlayerData>& m_players;

    int m_difficulty = 1;


public:
    AIcontrol(const std::vector<PlayerData>& players) : m_players(players)
    {
    }

    void Growth(Field& field, int m_playerID, int availableAnts)
    {
        m_decision = {};

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
        if (m_players[m_playerID].m_persona.growth == Aggressive)
        {
            candidates = Filter(candidates, [&](int row, int col)
                {
                    auto adjacent = FindAdjacentCandidates(
                        field, row, col, [&](int r, int c)
                        {
                            int owner = field.getOwner(r, c);

                            return owner != 0 && owner != m_playerID;
                        });

                    return !adjacent.empty();
                });
        }
        else if (m_players[m_playerID].m_persona.growth == Defensive)
        {
            candidates = Filter(candidates, [&](int row, int col)
                {
                    auto adjacent = FindAdjacentCandidates(
                        field, row, col, [&](int r, int c)
                        {
                            return field.getOwner(r, c) == 0;
                        });

                    return !adjacent.empty();
                });
        }

        if (candidates.empty())
        {
            return;
        }

        auto bestCandidates = Best( candidates, [&](int row, int col)
            {
                return field.getStr(row, col);
            });

        std::uniform_int_distribution<int> dist( 0,
            static_cast<int>(bestCandidates.size()) - 1);

        const Candidate& target = bestCandidates[dist(gRNG)];

        m_decision.srow = target.row;
        m_decision.scol = target.col;
        m_decision.trow = target.row;
        m_decision.tcol = target.col;
        m_decision.tant = availableAnts;

        return;
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
            m_decision.sant = AntSelect(available);

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
        m_decision.tant = AntSelect(m_decision.sant);

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
            sources = Filter(sources, [&](int row, int col)
                {
                    auto adjacent = FindAdjacentCandidates(
                        field, row, col, [&](int r, int c)
                        {
                            int owner = field.getOwner(r, c);

                            return owner != 0 && owner != m_playerID;
                        });

                    return !adjacent.empty();
                });
        }
        else if (m_players[m_playerID].m_persona.attack == Defensive)
        {
            sources = Filter(sources, [&](int row, int col)
                {
                    auto adjacent = FindAdjacentCandidates(
                        field, row, col, [&](int r, int c)
                        {
                            return field.getOwner(r, c) == 0;
                        });

                    return !adjacent.empty();
                });
        }
        if (sources.empty())
        {   // Forced Skip: No valid source cells
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
        m_decision.sant = AntSelect(
            field.getAnts(source.row, source.col) - 1);

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

    }

    // Getters
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

};

