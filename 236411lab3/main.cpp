#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <chrono> 
#include <fstream>

//sterowanie symulacja
const int storageSize= 8192;//512*4=2048, 1024*4=4096, 2048*4=8192
bool localSIMD= true;

float storageOfDataA[storageSize][4];
float storageOfDataB[storageSize][4];
float storageOfResults[storageSize][4];

//Pozostalosci z testowania funkcji
//float a[4] = {1,9,9,7};
//float b[4] = {2,0,2,1};
//float sum[4] = {4,1,9,6};
//float sumX[10][4];
//float result[4] ={0,0,0,0};

void sumSIMD(int index){//funkcja przesz test
    __asm__ __volatile__(
        "MOVUPS (%0), %%xmm0\n"
        "MOVUPS (%1), %%xmm1\n"
        //"SUBPS %%xmm3, %%xmm1\n"  // xmm1-xmm3 w xmm1
        "ADDPS %%xmm1, %%xmm0\n" // w xmm0
        "MOVUPS %%xmm0, (%2)"
        :
        //: "r"(a), "r"(b), "r" (sum)
        : "r"(storageOfDataA[index]), "r"(storageOfDataB[index]), "r" (storageOfResults[index])
    );
}

void subSIMD(int index){
    __asm__ __volatile__(
        "MOVUPS (%0), %%xmm0\n"
        "MOVUPS (%1), %%xmm1\n"
        "SUBPS %%xmm1, %%xmm0\n"
        "MOVUPS %%xmm0, (%2)"
        :
        : "r"(storageOfDataA[index]), "r"(storageOfDataB[index]), "r" (storageOfResults[index])
    );
}

void mulSIMD(int index){
    __asm__ __volatile__(
        "MOVUPS (%0), %%xmm0\n"
        "MOVUPS (%1), %%xmm1\n"
        "MULPS %%xmm1, %%xmm0\n"
        "MOVUPS %%xmm0, (%2)"
        :
        : "r"(storageOfDataA[index]), "r"(storageOfDataB[index]), "r" (storageOfResults[index])
    );
}

void divSIMD(int index){
    __asm__ __volatile__(
        "MOVUPS (%0), %%xmm0\n"
        "MOVUPS (%1), %%xmm1\n"
        "DIVPS %%xmm1, %%xmm0\n"
        "MOVUPS %%xmm0, (%2)"
        :
        : "r"(storageOfDataA[index]), "r"(storageOfDataB[index]), "r" (storageOfResults[index])
    );
}

void initializeData(){
    srand(time(NULL));
    for(int i=0;i<storageSize;i++){
        for(int j=0;j<4;j++){
            storageOfDataA[i][j]= (rand() %1000)+1; //chwilowe uproszczenie zeby nie bylo dzielenia przez 0
            storageOfDataB[i][j]= (rand() %1000)+1; 
        }
    }
}

void sumSISD(int index){
    for(int j=0;j<4;j++){
        storageOfResults[index][j]=storageOfDataA[index][j]+storageOfDataB[index][j];
    }
}
void subSISD(int index){
    for(int j=0;j<4;j++){
        storageOfResults[index][j]=storageOfDataA[index][j]-storageOfDataB[index][j];
    }
}
void mulSISD(int index){
    for(int j=0;j<4;j++){
        storageOfResults[index][j]=storageOfDataA[index][j]*storageOfDataB[index][j];
    }
}
void divSISD(int index){
    for(int j=0;j<4;j++){
        storageOfResults[index][j]=storageOfDataA[index][j]/storageOfDataB[index][j];
    }
}




void iterate(bool isTypeSIMD, int calculationType){
    //do maths
    if(isTypeSIMD){
        //SIMD calculations
        switch(calculationType){
            case 0:
                for(int i=0;i<storageSize;i++){
                    sumSIMD(i);
                }
                break;
            case 1:
                for(int i=0;i<storageSize;i++){
                    subSIMD(i);
                }
                break;
            case 2:
                for(int i=0;i<storageSize;i++){
                    mulSIMD(i);
                }
                break;
            case 3:
                for(int i=0;i<storageSize;i++){
                    divSIMD(i);
                }
                break;
            default:
                break;
        }
    }else{
        //SISD calculations
        switch(calculationType){
            case 0://"+"
                for(int i=0;i<storageSize;i++){
                    sumSISD(i);
                }
                break;
            case 1://"-"
                for(int i=0;i<storageSize;i++){
                    subSISD(i);
                }
                break;

            case 2://"*"
                for(int i=0;i<storageSize;i++){
                    mulSISD(i);
                }
                break;

            case 3://"/"
                for(int i=0;i<storageSize;i++){
                    divSISD(i);
                }
                break;
            default:
                break;
        }
    }
}

void prepare_txt_file(int addTime, int subTime, int mulTime, int divTime,bool isTypeSIMD){
    std::ofstream outfile;
    outfile.open("results.txt", std::ios_base::app);//std::ios_base::app
    outfile << "Typ obliczen: ";
    if(isTypeSIMD){
        outfile<<"SIMD\n";
    }else{
        outfile<<"SISD\n";
    }
    outfile<<"Liczba liczb: "<<storageSize* 4<<std::endl;
    outfile<<"Sredni czas [mikro s]: "<<std::endl;
    outfile<<"+ "<<addTime<<std::endl;
    outfile<<"- "<<subTime<<std::endl;
    outfile<<"* "<<mulTime<<std::endl;
    outfile<<"/ "<<divTime<<std::endl;   
}

void simulation(){
    std::ofstream outfile;
    outfile.open("results.txt", std::ios_base::app);//std::ios_base::app
    bool controlSIMD= true;
    int controlNumber= 0;
    outfile<<storageSize<<std::endl;

    controlSIMD= true;

    outfile<<"+"<<std::endl;
    for(int k=0;k<10;k++){
        initializeData();
        auto start = std::chrono::high_resolution_clock::now();
        iterate(controlSIMD,controlNumber);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
        int addTime=duration.count();
        outfile<<addTime<<std::endl;
    }
    outfile<<std::endl;

    controlNumber= 1;
    outfile<<"-"<<std::endl;
    for(int k=0;k<10;k++){
        initializeData();
        auto start = std::chrono::high_resolution_clock::now();
        iterate(controlSIMD,controlNumber);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
        int addTime=duration.count();
        outfile<<addTime<<std::endl;
    }
    outfile<<std::endl;

    controlNumber= 2;
    outfile<<"*"<<std::endl;
    for(int k=0;k<10;k++){
        initializeData();
        auto start = std::chrono::high_resolution_clock::now();
        iterate(controlSIMD,controlNumber);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
        int addTime=duration.count();
        outfile<<addTime<<std::endl;
    }
    outfile<<std::endl;

    controlNumber= 3;
    outfile<<"/"<<std::endl;
    for(int k=0;k<10;k++){
        initializeData();
        auto start = std::chrono::high_resolution_clock::now();
        iterate(controlSIMD,controlNumber);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
        int addTime=duration.count();
        outfile<<addTime<<std::endl;
    }
    outfile<<std::endl;
    
    controlSIMD=false;

    controlNumber= 0;
    outfile<<"+SISD"<<std::endl;
    for(int k=0;k<10;k++){
        initializeData();
        auto start = std::chrono::high_resolution_clock::now();
        iterate(controlSIMD,controlNumber);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
        int addTime=duration.count();
        outfile<<addTime<<std::endl;
    }
    outfile<<std::endl;

    controlNumber= 1;
    outfile<<"-SISD"<<std::endl;
    for(int k=0;k<10;k++){
        initializeData();
        auto start = std::chrono::high_resolution_clock::now();
        iterate(controlSIMD,controlNumber);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
        int addTime=duration.count();
        outfile<<addTime<<std::endl;
    }
    outfile<<std::endl;

    controlNumber= 2;
    outfile<<"*SISD"<<std::endl;
    for(int k=0;k<10;k++){
        initializeData();
        auto start = std::chrono::high_resolution_clock::now();
        iterate(controlSIMD,controlNumber);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
        int addTime=duration.count();
        outfile<<addTime<<std::endl;
    }
    outfile<<std::endl;

    controlNumber= 3;
    outfile<<"/SISD"<<std::endl;
    for(int k=0;k<10;k++){
        initializeData();
        auto start = std::chrono::high_resolution_clock::now();
        iterate(controlSIMD,controlNumber);
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
        int addTime=duration.count();
        outfile<<addTime<<std::endl;
    }
    outfile<<std::endl;

}



int main(){
    initializeData();

    //simulation();

    bool isTypeSIMD = localSIMD;
    int addTime, subTime, mulTime, divTime;

    auto start = std::chrono::high_resolution_clock::now();
    iterate(isTypeSIMD,0);
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    addTime=duration.count();

    start = std::chrono::high_resolution_clock::now();
    iterate(isTypeSIMD,1);
    finish = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    subTime=duration.count();

    start = std::chrono::high_resolution_clock::now();
    iterate(isTypeSIMD,2);
    finish = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    mulTime=duration.count();

    start = std::chrono::high_resolution_clock::now();
    iterate(isTypeSIMD,3);
    finish = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    divTime=duration.count();

    prepare_txt_file(addTime,subTime,mulTime,divTime,isTypeSIMD);


    return 0;
}
