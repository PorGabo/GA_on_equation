
struct life
{
    vector<generation> generations;

    life(int num)
    {
        generations.push_back(generation());
        
        for (int i = 1; i <= num; i++)
        {
            generations.push_back(generation(generations[i - 1].nextx, generations[i - 1].nexty));
        }
    }
};
