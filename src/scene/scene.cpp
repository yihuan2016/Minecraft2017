#include <scene/scene.h>
#include <scene/cube.h>
#include <time.h>
#include <random>

#define BOUNDRY 160
#define M_PI  3.14159
const int Step=2;
const int seaLevel=79;

Block::Block() : pos(0,0,0), m_active(true), blockType(Default)
{
}

bool Block::isActive()
{
    return m_active;
}

void Block::SetActive(bool active)
{
    m_active = active;
}

Scene::Scene() : dimensions(BOUNDRY, BOUNDRY, BOUNDRY), lowerBoundX(0), higherBoundX(BOUNDRY), lowerBoundY(0), \
    higherBoundY(BOUNDRY), lowerBoundZ(0), higherBoundZ(BOUNDRY)
{
    for (int x = 0; x < BOUNDRY; x++)
        for (int y = 0; y < BOUNDRY; y++)
            for (int z = 0; z <BOUNDRY; z++)
            {
                pos = std::make_tuple(x,y,z);
                containBlock[pos] = false;
//                scene[pos] = Default;
            }

    river1=River(10,35,1);
    river2=River(40,130,2);
}

void Scene::CreateTestScene()
{
    Perlin* perlin_generator = new Perlin();
    srand (time(NULL));


    for (int x = lowerBoundX; x < higherBoundX; x++)
        for(int z = lowerBoundZ; z < higherBoundZ; z++)
        {
            int height = perlin_generator->OctavePerlin((float)x/BOUNDRY, 0, (float)z/BOUNDRY, 6, 1) * BOUNDRY;

            // height field is around ~80 with 16 as variance
            // terrain part + part between sea level and bedrock
            // COORDINATE: bedrock: 16, sea level: 80, above sea level(terrain): 160

//            // generate snow effect, Y: height~160
//            pos = std::make_tuple(x, height + 10 + clock() % 30, z);
//            containBlock[pos] = true;
//            scene[pos] = Snow;

            // generate Ice Plains Spikes, Y max: height+27
            for (int count = 0; count < 4; count++)
            {
                if ((x >= (lowerBoundX+higherBoundX)/2- 7*count - 21) && (x <= (lowerBoundX+higherBoundX)/2 - 7*count - 18) &&
                        (z >= (lowerBoundZ+higherBoundZ)/2 - 7*count - 21) && (z <= (lowerBoundZ+higherBoundZ)/2 - 7*count - 18))
                {
                    int spikeHeight = (int)((double)rand()/RAND_MAX*10); // random value between 0 to 10
                    for (int i = height+16; i < height + 16 + spikeHeight; i++)
                    {
                        pos = std::make_tuple(x, i, z);
                        containBlock[pos] = true;
                        scene[pos] = Lava;
                    }
                }
            }


//            // generate grass
//            if (x >= (lowerBoundX+higherBoundX)/3-3 && x <= (lowerBoundX+higherBoundX)/3+3 && z >= (lowerBoundZ+higherBoundZ)/3-3 && z <= (lowerBoundZ+higherBoundZ)/3+3)
//            {
//                int grassHeight = (int)((double)rand()/RAND_MAX*3); // random value between 0 to 2
//                pos = std::make_tuple(x, height+16+grassHeight, z);
//                containBlock[pos] = true;
//                scene[pos] = Grass;
//            }

            //generate Ice Plains
            pos = std::make_tuple(x, height+15, z);
            containBlock[pos] = true;
            scene[pos] = Snow;

//            // generate grassland
//            pos = std::make_tuple(x, height+15, z);
//            containBlock[pos] = true;
//            scene[pos] = GrassLand;

            // terrain
            for (int i = 64; i < height+15; i++)
            {
                pos = std::make_tuple(x, i, z);
                containBlock[pos] = true;
                scene[pos] = Default;
            }

            // stone
            for (int i = 16; i < 64; i++)
            {
                pos = std::make_tuple(x, i, z);
                containBlock[pos] = true;
                scene[pos] = Stone;
            }
            // At a depth of 128 blocks, a layer of BEDROCK should be generated;
            for (int i = 0; i < 16; i++)
            {
                pos = std::make_tuple(x, i, z);
                containBlock[pos] = true;
                scene[pos] = BedRock;
            }

        }

    // generate random underground x, y, z pos
    int stride = 1;
    int radius = 1;
    int minLength = 10;
    int maxLength = 30;
    int maxCount = maxLength / minLength;
    double randomEllipsoid = 0;
    double randomCoreIron = 0;
    int numberOfTunnels = (double) rand() / RAND_MAX * 5 + 1; // random number between 1-5;
    int currentTunnelNumber = 0;

    // generate caves and tunnels
    while (currentTunnelNumber < numberOfTunnels)
    {
        int x_pos = BOUNDRY/2 - (double)rand() / RAND_MAX * 50;
        int y_pos = BOUNDRY/2 - (double)rand() / RAND_MAX * 20;
        int z_pos = BOUNDRY/2 - (double)rand() / RAND_MAX * 50;
        int count = 0;
        bool ellipsoidUsed = false;


        double walkLength = (double)rand() / RAND_MAX;
        printf("walklength is : %f\n", walkLength);

        while (walkLength <= 0.5 && count <= maxCount)
        {
            for (int i = 0; i < minLength; i++)
            {
                double angle = perlin_generator->OctavePerlin((float)x_pos, (float)y_pos, (float)z_pos, 6, 1) *M_PI ;
                printf("sin: %f, cos: %f\n", sin(angle), cos(angle));
                if (cos(angle) <= 0)
                    x_pos -= stride * 4;
                else
                    x_pos += stride * 4;
                if (sin(angle) <= 0)
                    z_pos -= stride * 4;
                else
                    z_pos += stride * 4;

                y_pos -= stride;
                radius = ((double)rand() / RAND_MAX) * 2 + 2; // random value of 2,3

                randomEllipsoid = (double)rand() / RAND_MAX;

                printf("randomEllipsoid is : %f\n", randomEllipsoid);


                if (randomEllipsoid >= 0.5 && ellipsoidUsed == false)
                {
                    // hollow ellipsoid cave & make LAVA blocks、
                    printf("creating cave\n");

                    int x_pos_random = (double) rand()/RAND_MAX * 4;
                    int y_pos_random = (double) rand()/RAND_MAX * 4;
                    int z_pos_random = (double) rand()/RAND_MAX * 4;

                    for (int y = y_pos - y_pos_random; y <= y_pos + y_pos_random; y++)
                    {
                        // bottom layer should be LAVA.
                        if (y == y_pos - y_pos_random)
                        {
                            for (int x = x_pos - x_pos_random; x <= x_pos + x_pos_random; x++)
                                for (int z = z_pos - z_pos_random; z <= z_pos + z_pos_random; z++)
                                {
                                    pos = std::make_tuple(x, y, z);
                                    containBlock[pos] = true;
                                    scene[pos] = Lava;
                                }

                        }
                        else
                        {
                            for (int x = x_pos - x_pos_random; x <= x_pos + x_pos_random; x++)
                                for (int z = z_pos - z_pos_random; z <= z_pos + z_pos_random; z++)
                                {
                                    pos = std::make_tuple(x, y, z);

                                    randomCoreIron = (double) rand() / RAND_MAX;

                                    if (randomCoreIron > 0.9)
                                    {
                                        containBlock[pos] = true;
                                        scene[pos] = IronOre;
                                    }
                                    else if (randomCoreIron > 0.8)
                                    {
                                        containBlock[pos] = true;
                                        scene[pos] = Coal;
                                    }
                                    else
                                        containBlock[pos] = false;
                                }
                        }

                        x_pos_random = (double) rand()/RAND_MAX * 4;
                        z_pos_random = (double) rand()/RAND_MAX * 4;
                    }
                    ellipsoidUsed = true;
                }

                // tunnel create
                else
                {
                    for (int x = x_pos - radius; x <= x_pos + radius; x++)
                        for (int y = y_pos - radius; y <= y_pos + radius; y++)
                            for (int z = z_pos - radius; z <= z_pos + radius; z++)
                            {
                                pos = std::make_tuple(x, y, z);
                                // randomly distribute COAL and IRON ORE blocks throughout your tunnels and large caves
                                randomCoreIron = (double) rand() / RAND_MAX;
                                if (randomCoreIron > 0.9)
                                {
                                    containBlock[pos] = true;
                                    scene[pos] = IronOre;
                                }
                                else if (randomCoreIron > 0.8)
                                {
                                    containBlock[pos] = true;
                                    scene[pos] = Coal;
                                }
                                else
                                    containBlock[pos] = false;
                            }
                }

                printf("%d %d %d\n", x_pos, y_pos, z_pos);


            }
    //         generate random number between 0 to 1 to determine if we should halt walking
            walkLength = (double) rand() / RAND_MAX;
            randomEllipsoid = (double) rand() / RAND_MAX;
            count += 1;
        }

        currentTunnelNumber++;
    }

    UpdateRiver();


}


void Scene::UpdateRiver()
{
    for (int x = lowerBoundX; x < higherBoundX; x++)
    {
        for(int z = lowerBoundZ; z < higherBoundZ; z++)
        {
            std::tuple<int,int,int> posr = std::make_tuple(x, 0, z);
            if((river1.containRiver[posr]==true)&&
                   (containBlock[posr]==true)&&(scene[posr]!=Water))
            {
                MakeWater(x,0+seaLevel,z);
            }
            if((river2.containRiver[posr]==true)&&
                    (containBlock[posr]==true)&&(scene[posr]!=Water))
            {
                MakeWater(x,0+seaLevel,z);
            }
        }

    }
    GenerateValley();
}

void Scene::GenerateValley()
{
    for (int x = lowerBoundX; x < higherBoundX; x++)
    {
        for(int z = lowerBoundZ; z < higherBoundZ; z++)
        {
            std::tuple<int,int,int> posr = std::make_tuple(x, 0, z);
            if((containBlock[posr]==true)&&(scene[posr]==Water))
            {


                //int minuslimit=z-lowerBoundZ;
                //int addlimit=higherBoundZ-z-1;
                int minuslimit=50;
                int addlimit=50;
                int mht=0+seaLevel;
                int aht=0+seaLevel;
                std::tuple<int,int,int> posZ_1 = std::make_tuple(x, 0, z-1);
                std::tuple<int,int,int> posZa_1 = std::make_tuple(x, 0, z+1);
                if((containBlock[posZ_1]==true)&&(scene[posZ_1]!=Water))
                {
                for(int minusfactor=1;minusfactor<=minuslimit;minusfactor++)
                {
                    if(minusfactor>=z)
                        break;
                    std::tuple<int,int,int> posnear = std::make_tuple(x, 0, z-minusfactor);
                    //mht++;
                    mht+=Step;
                    if((containBlock[posnear]==true)&&(scene[posnear]!=Water))
                    {
                        int maxyn=0;
                        for(int yn=lowerBoundY;yn<higherBoundY;yn++)
                        {
                            if(containBlock[std::make_tuple(x,yn,z-minusfactor)]==true)
                            {
                                if(yn>maxyn)
                                    maxyn=yn;
                            }
                        }
                        if(maxyn>mht)
                        {
                            for(int yn=lowerBoundY;yn<higherBoundY;yn++)
                            {
                                std::tuple<int,int,int> posyn = std::make_tuple(x, yn, z-minusfactor);
                                if(yn>mht)
                                    containBlock[posyn]=false;
//                                else
//                                {containBlock[posyn]=true;
//                                scene[posyn]=Sand;}

                            }
                        }

                    }else
                        break;
                }
                }
                if((containBlock[posZa_1]==true)&&(scene[posZa_1]!=Water)){
                for(int addfactor=1;addfactor<addlimit;addfactor++)
                {
                    if(z+addfactor>=higherBoundZ)
                        break;
                    std::tuple<int,int,int> posnear = std::make_tuple(x, 0, z+addfactor);
                    //aht++;
                    aht+=Step;
                    if((containBlock[posnear]==true)&&(scene[posnear]!=Water))
                    {
                        int maxyn=0;
                        for(int yn=lowerBoundY;yn<higherBoundY;yn++)
                        {
                            if(containBlock[std::make_tuple(x,yn,z+addfactor)]==true)
                            {
                                if(yn>maxyn)
                                    maxyn=yn;
                            }
                        }
                        if(maxyn>aht)
                        {
                            for(int yn=lowerBoundY;yn<higherBoundY;yn++)
                            {
                                std::tuple<int,int,int> posyn = std::make_tuple(x, yn, z+addfactor);
                                if(yn>aht)
                                    containBlock[posyn]=false;
//                                else
//                                {containBlock[posyn]=true;
//                                scene[posyn]=Sand;}

                            }
                        }

                    }else
                        break;
                }
                }




                int xmht=0+seaLevel;
                int xaht=0+seaLevel;
                int type=0;
                std::tuple<int,int,int> posx_1 = std::make_tuple(x-1, 0, z);
                //std::tuple<int,int,int> posxx_1 = std::make_tuple(x-1, 0, z-1);
                std::tuple<int,int,int> posxa_1 = std::make_tuple(x+1, 0, z);
                //std::tuple<int,int,int> posxxa_1 = std::make_tuple(x+1, 0, z-1);
                if((containBlock[posx_1]==true)&&(scene[posx_1]!=Water))
                {
                    type=0;
                    std::tuple<int,int,int> posxx_1 = std::make_tuple(x, 0, z-1);
                    std::tuple<int,int,int> posxxx_1 = std::make_tuple(x, 0, z+1);
                    if((containBlock[posxx_1]==true)&&(scene[posxx_1]!=Water))
                            type=1;
                    if((containBlock[posxxx_1]==true)&&(scene[posxxx_1]!=Water))
                            type=2;
                for(int xminusfactor=1;xminusfactor<=minuslimit;xminusfactor++)
                {
                    if(xminusfactor>=x)
                        break;
                    std::tuple<int,int,int> posnear = std::make_tuple(x-xminusfactor, 0, z);
                    //xmht++;
                    xmht+=Step;
                    if((containBlock[posnear]==true)&&(scene[posnear]!=Water))
                    {
                        int maxyn=0;
                        for(int yn=lowerBoundY;yn<higherBoundY;yn++)
                        {
                            if(containBlock[std::make_tuple(x-xminusfactor,yn,z)]==true)
                            {
                                if(yn>maxyn)
                                    maxyn=yn;
                            }
                        }
                        if(maxyn>xmht)
                        {
                            for(int yn=lowerBoundY;yn<higherBoundY;yn++)
                            {
                                std::tuple<int,int,int> posyn = std::make_tuple(x-xminusfactor, yn, z);
                                if(yn>xmht)
                                    containBlock[posyn]=false;
//                                else
//                                {containBlock[posyn]=true;
//                                 scene[posyn]=Sand;
//                                }

                            }
                            if(type==1)
                                GenerateValleyHelper(x-xminusfactor,xmht,z);
                            if(type==2)
                                GenerateValleyHelper(x-xminusfactor,xmht,z);
                        }

                    }else
                        break;
                }
                }
                if((containBlock[posxa_1]==true)&&(scene[posxa_1]!=Water))
                {
                    type=0;
                    std::tuple<int,int,int> posxx_1 = std::make_tuple(x, 0, z-1);
                    std::tuple<int,int,int> posxxx_1 = std::make_tuple(x, 0, z+1);
                    if((containBlock[posxx_1]==true)&&(scene[posxx_1]!=Water))
                            type=1;
                    if((containBlock[posxxx_1]==true)&&(scene[posxxx_1]!=Water))
                            type=2;
                for(int addfactor=1;addfactor<addlimit;addfactor++)
                {
                    if(addfactor+x>higherBoundX)
                        break;
                    std::tuple<int,int,int> posnear = std::make_tuple(x+addfactor, 0, z);
                    //xaht++;
                    xaht+=Step;
                    if((containBlock[posnear]==true)&&(scene[posnear]!=Water))
                    {
                        int maxyn=0;
                        for(int yn=lowerBoundY;yn<higherBoundY;yn++)
                        {
                            if(containBlock[std::make_tuple(x+addfactor,yn,z)]==true)
                            {
                                if(yn>maxyn)
                                    maxyn=yn;
                            }
                        }
                        if(maxyn>xaht)
                        {
                            for(int yn=lowerBoundY;yn<higherBoundY;yn++)
                            {
                                std::tuple<int,int,int> posyn = std::make_tuple(x+addfactor, yn, z);
                                if(yn>xaht)
                                    containBlock[posyn]=false;
//                                else
//                                {containBlock[posyn]=true;
//                                scene[posyn]=Sand;
//                                }

                            }
                            if(type==1)
                                GenerateValleyHelper(x+addfactor,xaht,z);
                            if(type==2)
                                GenerateValleyHelper(x+addfactor,xaht,z);
                        }

                    }else
                        break;
                }
                }


            }
        }

    }
}

int Scene::GetYMax(int x, int z)
{
    int maxy=0;
    for(int y=lowerBoundY;y<higherBoundY;y++)
    {
        if(containBlock[std::make_tuple(x,y,z)]==true)
        {
            if(maxy<y)
                maxy=y;
        }
    }
    return maxy;
}
void Scene::MakeWater(int x, int y, int z)
{
    for(int yn=lowerBoundY;yn<higherBoundY;yn++)
    {
        if(yn>y)
            containBlock[std::make_tuple(x,yn,z)]=false;
        else
        {    containBlock[std::make_tuple(x,yn,z)]=true;
             scene[std::make_tuple(x,yn,z)]=Water;
        }
    }
}
void Scene::GenerateValleyHelper(int x, int y, int z)
{
    int minuslimit=50;
    int addlimit=50;
    int mht=y;
    int aht=y;

    std::tuple<int,int,int> posZ_1 = std::make_tuple(x, 0, z-1);
    std::tuple<int,int,int> posZa_1 = std::make_tuple(x, 0, z+1);

    if((containBlock[posZ_1]==true)&&(scene[posZ_1]!=Water))
    {
    for(int minusfactor=1;minusfactor<=minuslimit;minusfactor++)
    {
        if(minusfactor>=z)
            break;
        std::tuple<int,int,int> posnear = std::make_tuple(x, 0, z-minusfactor);
        //mht++;
        mht+=Step;
        if((containBlock[posnear]==true)&&(scene[posnear]!=Water))
        {
            int maxyn=0;
            for(int yn=lowerBoundY;yn<higherBoundY;yn++)
            {
                if(containBlock[std::make_tuple(x,yn,z-minusfactor)]==true)
                {
                    if(yn>maxyn)
                        maxyn=yn;
                }
            }
            if(maxyn>mht)
            {
                for(int yn=lowerBoundY;yn<higherBoundY;yn++)
                {
                    std::tuple<int,int,int> posyn = std::make_tuple(x, yn, z-minusfactor);
                    if(yn>mht)
                        containBlock[posyn]=false;
//                    else
//                    {containBlock[posyn]=true;
//                    scene[posyn]=Sand;}

                }
            }

        }else
            break;
    }
    }
    if((containBlock[posZa_1]==true)&&(scene[posZa_1]!=Water)){
    for(int addfactor=1;addfactor<addlimit;addfactor++)
    {
        if(z+addfactor>=higherBoundZ)
            break;
        std::tuple<int,int,int> posnear = std::make_tuple(x, 0, z+addfactor);
        //aht++;
        aht+=Step;
        if((containBlock[posnear]==true)&&(scene[posnear]!=Water))
        {
            int maxyn=0;
            for(int yn=lowerBoundY;yn<higherBoundY;yn++)
            {
                if(containBlock[std::make_tuple(x,yn,z+addfactor)]==true)
                {
                    if(yn>maxyn)
                        maxyn=yn;
                }
            }
            if(maxyn>aht)
            {
                for(int yn=lowerBoundY;yn<higherBoundY;yn++)
                {
                    std::tuple<int,int,int> posyn = std::make_tuple(x, yn, z+addfactor);
                    if(yn>aht)
                        containBlock[posyn]=false;
//                    else
//                    {containBlock[posyn]=true;
//                    scene[posyn]=Sand;}

                }
            }

        }else
            break;
    }
    }
}
