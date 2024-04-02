#ifndef POKEPARSER_H
#define POKEPARSER_H
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
class pokemon{
    private:
        
    public:
        int id, species_id,height,weight,base_experience,order,is_default;
        std::string identifier;
        pokemon();
        pokemon(int iD, std::string i, int sid,int h, int w, int be, int o, int isd);

};
class moves{};
class pokemon_moves{};
class pokemon_species{};
class experience{};
class type_names{};
class pokemon_stats{};
class stats{};
class pokemon_types{};

class pokeparser
{
private:
    
public:
    pokeparser();
    ~pokeparser();
    std::vector<pokemon> parsePokemon();
    std::string pathOne;
    std::string pathTwo;
     std::string pathThree;

};



#endif