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
        std::cout << "path 1 not open: " << pathOne << std::endl;
        file.open(pathTwo + "pokemon.csv");
        if (!file.is_open())
        {
            std::cout << "path 2 not open: " << pathTwo<<std::endl;
            file.open(pathThree + "pokemon.csv");
            if (!file.is_open())
            {
                std::cout << "path 3 not open: "<<pathThree << std::endl;
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
        int values[7];
        std::fill(std::begin(values),std::end(values),INT_MAX);
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
        std::cout << "path 1 not open: " << pathOne << std::endl;
        file.open(pathTwo + "moves.csv");
        if (!file.is_open())
        {
            std::cout << "path 2 not open: " << pathTwo<<std::endl;
            file.open(pathThree + "moves.csv");
            if (!file.is_open())
            {
                std::cout << "path 3 not open: "<<pathThree << std::endl;
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
        int values[14];
        std::fill(std::begin(values),std::end(values),INT_MAX);

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
        std::cout << "path 1 not open: " << pathOne << std::endl;
        file.open(pathTwo + "pokemon_moves.csv");
        if (!file.is_open())
        {
            std::cout << "path 2 not open: " << pathTwo<<std::endl;
            file.open(pathThree + "pokemon_moves.csv");
            if (!file.is_open())
            {
                std::cout << "path 3 not open: "<<pathThree << std::endl;
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
        int values[6];
        std::fill(std::begin(values),std::end(values),INT_MAX);

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
std::vector<pokemon_species> pokeparser::parsePokemonSpecies(){
    std::vector<pokemon_species> moves_v;
    std::ifstream file(pathOne + "pokemon_species.csv");
    std::string line;
    if (!file.is_open())
    {
        std::cout << "path 1 not open: " << pathOne << std::endl;
        file.open(pathTwo + "pokemon_species.csv");
        if (!file.is_open())
        {
            std::cout << "path 2 not open: " << pathTwo<<std::endl;
            file.open(pathThree + "pokemon_species.csv");
            if (!file.is_open())
            {
                std::cout << "path 3 not open: "<<pathThree << std::endl;
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
        int values[19];
        std::fill(std::begin(values),std::end(values),INT_MAX);

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
        moves_v.push_back(pokemon_species(values[0], identifier, values[1], values[2], values[3], values[4], values[5], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15], values[16], values[17],values[18]));
        ss.clear();
    }
    file.close();
    return moves_v;
}
std::vector<experience> pokeparser::parseExperience(){
    std::vector<experience> moves_v;
    std::ifstream file(pathOne + "experience.csv");
    std::string line;
    if (!file.is_open())
    {
        std::cout << "path 1 not open: " << pathOne << std::endl;
        file.open(pathTwo + "experience.csv");
        if (!file.is_open())
        {
            std::cout << "path 2 not open: " << pathTwo<<std::endl;
            file.open(pathThree + "experience.csv");
            if (!file.is_open())
            {
                std::cout << "path 3 not open: "<<pathThree << std::endl;
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
        int values[3];
        std::fill(std::begin(values),std::end(values),INT_MAX);
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
        moves_v.push_back(experience(values[0], values[1], values[2]));
        ss.clear();
    }
    file.close();
    return moves_v;
}
std::vector<type_names> pokeparser::parseTypeNames(){
    std::vector<type_names> moves_v;
    std::ifstream file(pathOne + "type_names.csv");
    std::string line;
    if (!file.is_open())
    {
        std::cout << "path 1 not open: " << pathOne << std::endl;
        file.open(pathTwo + "type_names.csv");
        if (!file.is_open())
        {
            std::cout << "path 2 not open: " << pathTwo<<std::endl;
            file.open(pathThree + "type_names.csv");
            if (!file.is_open())
            {
                std::cout << "path 3 not open: "<<pathThree << std::endl;
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
        int values[2];
        std::fill(std::begin(values),std::end(values),INT_MAX);

        std::string identifier;
        while (std::getline(ss, f, ','))
        {
            if (count == 2)
            {
                identifier = f;
            }
            else
            {
                try
                {
                    values[count] = std::stoi(f);
                }
                catch (const std::invalid_argument &e)
                {
                    values[count] = INT_MAX;
                }
            }
            count++;
        }
        moves_v.push_back(type_names(values[0], values[1],identifier));
        ss.clear();
    }
    file.close();
    return moves_v;
}
std::vector<pokemon_stats> pokeparser::parsePokemonStats(){
    std::vector<pokemon_stats> moves_v;
    std::ifstream file(pathOne + "pokemon_stats.csv");
    std::string line;
    if (!file.is_open())
    {
        std::cout << "path 1 not open: " << pathOne << std::endl;
        file.open(pathTwo + "pokemon_stats.csv");
        if (!file.is_open())
        {
            std::cout << "path 2 not open: " << pathTwo<<std::endl;
            file.open(pathThree + "pokemon_stats.csv");
            if (!file.is_open())
            {
                std::cout << "path 3 not open: "<<pathThree << std::endl;
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
        int values[4];
        std::fill(std::begin(values),std::end(values),INT_MAX);
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
        moves_v.push_back(pokemon_stats(values[0], values[1], values[2],values[3]));
        ss.clear();
    }
    file.close();
    return moves_v;
}
std::vector<stats> pokeparser::parseStats(){
    std::vector<stats> moves_v;
    std::ifstream file(pathOne + "stats.csv");
    std::string line;
    if (!file.is_open())
    {
        std::cout << "path 1 not open: " << pathOne << std::endl;
        file.open(pathTwo + "stats.csv");
        if (!file.is_open())
        {
            std::cout << "path 2 not open: " << pathTwo<<std::endl;
            file.open(pathThree + "stats.csv");
            if (!file.is_open())
            {
                std::cout << "path 3 not open: "<<pathThree << std::endl;
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
        int values[4];
        std::fill(std::begin(values),std::end(values),INT_MAX);

        std::string identifier;
        while (std::getline(ss, f, ','))
        {
            if (count == 2)
            {
                identifier = f;
            }
            else
            {
                try
                {   
                    if(count > 2)
                        values[count-1] = std::stoi(f);
                    else
                        values[count] = std::stoi(f);

                }
                catch (const std::invalid_argument &e)
                {
                     if(count > 2)
                        values[count-1] = INT_MAX;
                    else
                        values[count] = INT_MAX;
                }
            }
            count++;
        }
        moves_v.push_back(stats(values[0], values[1],identifier,values[2],values[3]));
        ss.clear();
    }
    file.close();
    return moves_v;
}
std::vector<pokemon_types> pokeparser::parsePokemonTypes(){
    std::vector<pokemon_types> moves_v;
    std::ifstream file(pathOne + "pokemon_types.csv");
    std::string line;
    if (!file.is_open())
    {
        std::cout << "path 1 not open: " << pathOne << std::endl;
        file.open(pathTwo + "pokemon_types.csv");
        if (!file.is_open())
        {
            std::cout << "path 2 not open: " << pathTwo<<std::endl;
            file.open(pathThree + "pokemon_types.csv");
            if (!file.is_open())
            {
                std::cout << "path 3 not open: "<<pathThree << std::endl;
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
        int values[3];
        std::fill(std::begin(values),std::end(values),INT_MAX);
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
        moves_v.push_back(pokemon_types(values[0], values[1], values[2]));
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
    std::cout << id << "," << identifier << ",";
    if(species_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<species_id<<",";}
    if(height == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<height<<",";}
    if(weight == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<weight<<",";}
    if(base_experience == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<base_experience<<",";}
    if(order == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<order<<",";}
    if(is_default == INT_MAX){
        std::cout<< " ";
    }else{std::cout<<is_default;}
    std::cout<<std::endl;
}
moves::moves() {}
moves::moves(int iD, std::string i, int gen, int type, int pwr, int p, int a, int pri, int tid, int dci, int eid, int ec, int cti, int ceid, int sceid)
    : id(iD), generation_id(gen), type_id(type), power(pwr), pp(p), accuracy(a),
      priority(pri), target_id(tid), damage_class_id(dci), effect_id(eid), effect_chance(ec),
      contest_type_id(cti), contest_effect_id(ceid), super_contest_effect_id(sceid), indentifier{i} {}
void moves::printMoves()
{
    std::cout << id << "," << indentifier << ",";
     if(generation_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<generation_id<<",";}
     if(type_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<type_id<<",";}
     if(power == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<power<<",";}
     if(pp == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<pp<<",";}
     if(accuracy == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<accuracy<<",";}
     if(priority == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<priority<<",";}
     if(target_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<target_id<<",";}
     if(damage_class_id == INT_MAX){
        std::cout<< " "<<" ";
    }else{std::cout<<damage_class_id<<",";}
     if(effect_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<effect_id<<",";}
     if(effect_chance == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<effect_chance<<",";}
     if(contest_type_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<contest_type_id<<",";}
     if(contest_effect_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<contest_effect_id<<",";}
     if(super_contest_effect_id == INT_MAX){
        std::cout<< " ";
    }else{std::cout<<super_contest_effect_id;}
    std::cout<<std::endl;
    
}
pokemon_moves::pokemon_moves(int pid, int vgid, int mid, int pmmid, int l, int o)
    : pokemon_id(pid), version_group_id(vgid), move_id(mid), pokemon_move_method_id(pmmid), level(l), order(o) {  }
pokemon_moves::pokemon_moves() {}
void pokemon_moves::printPokemonMoves()
{
     if(pokemon_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<pokemon_id<<",";}
     if(version_group_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<version_group_id<<",";}
     if(move_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<move_id<<",";}
     if(pokemon_move_method_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<pokemon_move_method_id<<",";}
     if(level == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<level<<",";}
     if(order == INT_MAX){
        std::cout<< " ";
    }else{std::cout<<order;}
    std::cout<<std::endl;

}
pokemon_species::pokemon_species(int iD,std::string i,int genid,int efsid,int evcid,int cid,int sid,int hid,int gr,int cr,int bh,int ib, int hc,int hgd,int grid,int fs,int il,int im,int o,int co)
        :id(iD),generation_id(genid),evolves_from_species_id(efsid),evolution_chain_id(evcid)
        ,color_id(cid),shape_id(sid),habitat_id(hid),gender_rate(gr),capture_rate(cr)
        ,base_happiness(bh),is_baby(ib),hatch_counter(hc),has_gender_differences(hgd)
        ,growth_rate_id(grid),forms_switchable(fs),is_legendary(il)
        ,is_mythical(im),order(o),conquest_order(co),indentifier(i){}
pokemon_species::pokemon_species(){}
void pokemon_species::printPokemonSpecies(){
    std::cout<<id<<","<< indentifier<< ",";
     if(generation_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<generation_id<<",";}
     if(evolves_from_species_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<evolves_from_species_id<<",";}
     if(evolution_chain_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<evolution_chain_id<<",";}
     if(color_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<color_id<<",";}
     if(shape_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<shape_id<<",";}
     if(habitat_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<habitat_id<<",";}
     if(gender_rate == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<gender_rate<<",";}
     if(capture_rate == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<capture_rate<<",";}
     if(base_happiness == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<base_happiness<<",";}
     if(is_baby == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<is_baby<<",";}
     if(hatch_counter == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<hatch_counter<<",";}
     if(has_gender_differences == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<has_gender_differences<<",";}
     if(growth_rate_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<growth_rate_id<<",";}
     if(forms_switchable == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<forms_switchable<<",";}
     if(is_legendary == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<is_legendary<<",";}
     if(is_mythical == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<is_mythical<<",";}
     if(order == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<order<<",";}
     if(conquest_order == INT_MAX){
        std::cout<< " ";
    }else{std::cout<<conquest_order;}
    std::cout<<std::endl;
}
experience::experience(int grid,int l,int e):growth_rate_id(grid),level(l),exp(e){}
experience::experience(){}
void experience::printExperience(){
     if(growth_rate_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<growth_rate_id<<",";}
     if(level == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<level<<",";}
     if(exp == INT_MAX){
        std::cout<< " ";
    }else{std::cout<<exp;}
    std::cout<<std::endl;
}
type_names::type_names(int tid,int llid,std::string n):type_id(tid),local_language_id(llid),name(n){}
type_names::type_names(){}
void type_names::printTypeNames(){
    if(type_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<type_id<<",";}
     if(local_language_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<local_language_id<<",";}
    std::cout<<name<<std::endl;
}
pokemon_stats::pokemon_stats(int pid,int sid,int bs,int e):pokemon_id(pid),stat_id(sid),base_stat(bs),effort(e){}
pokemon_stats::pokemon_stats(){}
void pokemon_stats::printPokemonStats(){
  if(pokemon_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<pokemon_id<<",";}
     if(stat_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<stat_id<<",";}
     if(base_stat == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<base_stat<<",";}
    if(effort == INT_MAX){
        std::cout<< " ";
    }else{std::cout<<effort;}
    std::cout<<std::endl;
}
stats::stats(int iD,int dcid,std::string i,int ibo,int gi):id(iD),damage_class_id(dcid),is_battle_only(ibo),game_index(gi),identifier(i){}
stats::stats(){}
void stats::printStats(){
    std::cout<<id<<",";
    if(damage_class_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<damage_class_id<<",";}
    std::cout<<identifier<<",";
     if(is_battle_only == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<is_battle_only<<",";}
     if(game_index == INT_MAX){
        std::cout<< " ";
    }else{std::cout<<game_index;}
    std::cout<<std::endl;
}
pokemon_types::pokemon_types(int pid,int tid,int s):pokemon_id(pid),type_id(tid),slot(s){}
pokemon_types::pokemon_types(){}
void pokemon_types::printPokemonTypes(){
    if(pokemon_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<pokemon_id<<",";}
     if(type_id == INT_MAX){
        std::cout<< " "<<",";
    }else{std::cout<<type_id<<",";}
     if(slot == INT_MAX){
        std::cout<< " ";
    }else{std::cout<<slot;}
    std::cout<<std::endl;
}

