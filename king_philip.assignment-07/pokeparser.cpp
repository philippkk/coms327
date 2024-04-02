#include "pokeparser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <climits>
std::vector<pokemon> pokeparser::parsePokemon()
{
    std::vector<pokemon> pokemon_v;
    std::ifstream file(pathOne + "pokemon.csv");
    std::string line;

    if (!file.is_open())
    {
        std::cout << "path 1 not open" << std::endl;
        file.open(pathTwo + "pokemon.csv");
        if (!file.is_open())
        {
            std::cout << "path 2 not open" << std::endl;
            file.open(pathThree + "pokemon.csv");
            if (!file.is_open())
            {
                std::cout << "path 3 not open" << std::endl;
                return pokemon_v;
            }
        }
    }
    int lineNum = 0;
    while (std::getline(file, line))
    {
        if (lineNum < 1)
        {
            lineNum++;
            continue;
        }
        std::stringstream ss(line);
        std::string f;
        int count = 0;
        int values[7] = {INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX};
        std::string identifier;
        while (std::getline(ss, f, ','))
        {
            if (count == 0)
            {
                try
                {
                    values[0] = std::stoi(f);
                }
                catch (const std::invalid_argument &e)
                {
                    values[0] = INT_MAX;
                }
            }
            else if (count == 1)
            {
                identifier = f;
            }
            else
            {
                try
                {
                    values[count - 1] = std::stoi(f);
                }
                catch (const std::invalid_argument &e)
                {
                    values[count - 1] = INT_MAX;
                }
            }
            count++;
        }
        pokemon_v.push_back(pokemon(values[0], identifier, values[1], values[2], values[3], values[4], values[5], values[6]));
        ss.clear();
    }
    file.close();
    return pokemon_v;
};
std::vector<moves> pokeparser::parseMoves()
{
    std::vector<moves> moves_v;
    std::ifstream file(pathOne + "moves.csv");
    std::string line;
    if (!file.is_open())
    {
        std::cout << "path 1 not open" << std::endl;
        file.open(pathTwo + "moves.csv");
        if (!file.is_open())
        {
            std::cout << "path 2 not open" << std::endl;
            file.open(pathThree + "moves.csv");
            if (!file.is_open())
            {
                std::cout << "path 3 not open" << std::endl;
                return moves_v;
            }
        }
    }
    int lineNum = 0;
    while (std::getline(file, line))
    {
        if (lineNum < 1)
        {
            lineNum++;
            continue;
        }
        std::stringstream ss(line);
        std::string f;
        int count = 0;
        int values[14]= {INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX};
        std::string identifier;
        while (std::getline(ss, f, ','))
        {
            if (count == 0)
            {
                try
                {
                    values[0] = std::stoi(f);
                }
                catch (const std::invalid_argument &e)
                {
                    values[0] = INT_MAX;
                }
            }
            else if (count == 1)
            {
                identifier = f;
            }
            else
            {
                try
                {
                    values[count - 1] = std::stoi(f);
                }
                catch (const std::invalid_argument &e)
                {
                    values[count - 1] = INT_MAX;
                }
            }
            count++;
        }
        moves_v.push_back(moves(values[0], identifier, values[1], values[2], values[3], values[4], values[5], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13]));
        ss.clear();
    }
    file.close();
    return moves_v;
};
std::vector<pokemon_moves> pokeparser::parsePokemonMoves()
{
    std::vector<pokemon_moves> moves_v;
    std::ifstream file(pathOne + "pokemon_moves.csv");
    std::string line;
    if (!file.is_open())
    {
        std::cout << "path 1 not open" << std::endl;
        file.open(pathTwo + "pokemon_moves.csv");
        if (!file.is_open())
        {
            std::cout << "path 2 not open" << std::endl;
            file.open(pathThree + "pokemon_moves.csv");
            if (!file.is_open())
            {
                std::cout << "path 3 not open" << std::endl;
                return moves_v;
            }
        }
    }
    int lineNum = 0;
    while (std::getline(file, line))
    {
        if (lineNum < 1)
        {
            lineNum++;
            continue;
        }
        std::stringstream ss(line);
        std::string f;
        int count = 0;
        int values[6] = {INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX};

        std::string identifier;
        while (std::getline(ss, f, ','))
        {
            try
            {
                values[count] = std::stoi(f);
            }
            catch (const std::invalid_argument &e)
            {
                values[count] = INT_MAX;
            }
            count++;
        }
        moves_v.push_back(pokemon_moves(values[0], values[1], values[2], values[3], values[4], values[5]));
        ss.clear();
    }
    file.close();
    return moves_v;
}
pokeparser::pokeparser()
{
    std::string home_dir = getenv("HOME");

    pathOne = "/share/cs327/pokedex/pokedex/data/csv/";
    pathTwo = home_dir + "/.poke327/pokedex/pokedex/data/csv/";
    pathThree = home_dir + "/Desktop/pokedex/pokedex/data/csv/";
}
pokeparser::~pokeparser() {}
pokemon::pokemon(int iD, std::string i, int sid, int h, int w, int be, int o, int isd) : id(iD), species_id(sid), height(h), weight(w), base_experience(be), order(o), is_default(isd), identifier(i)
{
}
pokemon::pokemon() {}
void pokemon::printPokemon()
{
    std::cout << id << " " << identifier << " " << species_id << " "
              << height << " " << weight << " " << base_experience << " " << order << " " << is_default << std::endl;
}
moves::moves() {}
moves::moves(int iD, std::string i, int gen, int type, int pwr, int p, int a, int pri, int tid, int dci, int eid, int ec, int cti, int ceid, int sceid)
    : id(iD), generation_id(gen), type_id(type), power(pwr), pp(p), accuracy(a),
      priority(pri), target_id(tid), damage_class_id(dci), effect_id(eid), effect_chance(ec),
      contest_type_id(cti), contest_effect_id(ceid), super_contest_effect_id(sceid), indentifier{i} {}
void moves::printMoves()
{
    std::cout << id << " " << indentifier << " " << generation_id << " " << type_id << " "
              << power << " " << pp << " " << accuracy << " " << priority << " " << target_id << " "
              << damage_class_id << " " << effect_id << " " << effect_chance << " " << contest_type_id << " "
              << contest_effect_id << " " << super_contest_effect_id << std::endl;
}
pokemon_moves::pokemon_moves(int pid, int vgid, int mid, int pmmid, int l, int o)
    : pokemon_id(pid), version_group_id(vgid), move_id(mid), pokemon_move_method_id(pmmid), level(l), order(o) {}
pokemon_moves::pokemon_moves() {}
void pokemon_moves::printPokemonMoves()
{
    std::cout << pokemon_id << " " << version_group_id << " " << move_id << " " << pokemon_move_method_id << " " << level << " " << order << std::endl;
}