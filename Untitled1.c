#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define A_ENTRY 32
#define FUSION(a,b) (a|(b<<8))

struct InfoSysterm
{
    uint16_t u16BytePerSector;
    uint8_t u8SecPerCluster;
    uint16_t u16SecInReserved;
    uint8_t u8numFatMapping;
    uint16_t u16numRootEntry;
    uint16_t u16numSecPerFat;
};
typedef struct InfoSysterm VALINFO;
VALINFO InfoVal;

struct Sector_Region
{
    uint8_t ReservedSec;
    uint32_t FATSec;
    uint32_t RootSec;
    uint32_t DataSec;
};
typedef struct Sector_Region SECRN;
SECRN SecBegin;

uint16_t ReadArrByte(FILE* FInP, uint16_t Pos, uint8_t numByte)
{
    uint8_t ArrBytePSec[numByte];
    uint16_t BytePSec;
    fseek(FInP, Pos, SEEK_SET );
    fread(ArrBytePSec,numByte,1,FInP);
    if(numByte == 2)
    {
        BytePSec = FUSION(ArrBytePSec[0],ArrBytePSec[1]);
    }
    else
    {
        BytePSec = ArrBytePSec[0];
    }
    return BytePSec;
}

void ConductInfoReserved(FILE* FInP)
{
    InfoVal.u8SecPerCluster = ReadArrByte(FInP, 0x00D, 1);
    printf("- have %d sector per cluster\n",InfoVal.u8SecPerCluster);
    InfoVal.u16SecInReserved = ReadArrByte(FInP, 0x00E, 2);
    printf("- have %d sector in reserved region\n",InfoVal.u8SecPerCluster);
    InfoVal.u8numFatMapping = ReadArrByte(FInP, 0x010, 1);
    printf("- have %d FAT mapping\n",InfoVal.u8numFatMapping);
    InfoVal.u16numRootEntry = ReadArrByte(FInP, 0x011, 2);
    printf("- have %d Entries each entry consits 32 bytes\n",InfoVal.u16numRootEntry);
    InfoVal.u16numSecPerFat = ReadArrByte(FInP, 0x016, 2);
    printf("- have %d Sector each FAT\n",InfoVal.u16numSecPerFat);
}

void PointToSector(uint16_t Sector,FILE* FInP)
{
    uint16_t address_start = InfoVal.u16BytePerSector*Sector;
    fseek(FInP, address_start, SEEK_SET );
}

void HandleSectorStart(void)
{
    SecBegin.ReservedSec = 0;
    printf("Reserved start at sector %d\n", SecBegin.ReservedSec);
    SecBegin.FATSec = InfoVal.u16SecInReserved;
    printf("Fat start at sector %d\n", SecBegin.FATSec);
    SecBegin.RootSec =  SecBegin.FATSec + InfoVal.u8numFatMapping*InfoVal.u16numSecPerFat;
    printf("Root start at sector %d\n", SecBegin.RootSec);
    SecBegin.DataSec = SecBegin.RootSec + InfoVal.u16numRootEntry*32/InfoVal.u16BytePerSector;
    printf("Data start at sector %d\n", SecBegin.DataSec);
}

/*void ParseRoot(FILE* FInP)
{
    PointToSector(SecBegin.RootSec,FInP)
}*/
int main()
{
   FILE *FInP;/*Khai bao file input*/
   uint8_t BeginFlag = 1;
   FInP = fopen("floppy.img","rb");
   if(FInP == NULL)
   {
      printf("Error input file\n");
      return(-1);
   }
   else
   {
   	   printf("Open file suscess\n");
   }
   if(BeginFlag == 1)
   {
       BeginFlag = 0;
       InfoVal.u16BytePerSector = ReadArrByte(FInP, 0x00B, 2);
       printf("- number byte per sector: %d\n",InfoVal.u16BytePerSector);
       ConductInfoReserved(FInP);
       HandleSectorStart();
       //char cArrSector[InfoVal.u16BytePerSector];
   }
   /*else
   {
       while(fread(cArrSector, InfoVal.u16BytePerSector, 1,FInP) != NULL)
       {
       	    if(ferror(FInP))
            {
    		    printf("Error read file\n");
    		    return(-1);
    		}
    		else
    		{
    		    iErFlag = iParseFile(cArrALine);
            }
       }
   }*/
   fclose(FInP);
   return(0);
}

