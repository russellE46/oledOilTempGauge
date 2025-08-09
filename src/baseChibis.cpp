#include "baseChibis.hpp"


/***************************************************************************************
 * @brief - decompressImage()
 *  Decompresses a run-length encoded image byte array into IMG_DECOMPRESSED
 * 
 *  NOT CURRENTLY WORKING
 * 
 * @return - None
 ***************************************************************************************/
void decompressImage(const unsigned char * CompImage)
{
    PTR_COMPRESSED_CHIBI ptrCompChibi = (PTR_COMPRESSED_CHIBI)CompImage;
    unsigned int size = 0, it = 0, decIt = 0;
    for (; it < ptrCompChibi->BytesOfSize; it++)
    {
        size += ptrCompChibi->Data[it];
    }

    while (it < size)
    {
        PTR_ENCODED_PAIR ptrEncPair = (ENCODED_PAIR*)(&ptrCompChibi->Data[it]);
        for (unsigned char i  = 0; i < ptrEncPair->Reps; i++)
        {
            // IMG_DECOMPRESSED[decIt] = ptrEncPair->Value;
            //IMG_DECOMPRESSED[decIt] = IMG_DECOMPRESSED[decIt];
            decIt++;
        }
        it += 2;
    }
}