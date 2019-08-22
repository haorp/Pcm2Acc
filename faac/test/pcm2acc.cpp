#include <faac.h>
#include <stdio.h>
 
typedef unsigned long   ULONG;
typedef unsigned int    UINT;
typedef unsigned char   BYTE;
typedef char            _TCHAR;
 
int main(int argc, _TCHAR* argv[])
{
    ULONG nSampleRate = 8000;  // 采样率
    UINT nChannels = 1;         // 声道数
    UINT nPCMBitSize = 16;      // 单样本位数
    ULONG nInputSamples = 0;
    ULONG nMaxOutputBytes = 0;
 
    int nRet;
    faacEncHandle hEncoder;
    faacEncConfigurationPtr pConfiguration; 
 
    int nBytesRead;
    int nPCMBufferSize;
    BYTE* pbPCMBuffer;
    BYTE* pbAACBuffer;
 
    FILE* fpIn; // PCM file for input
    FILE* fpOut; // AAC file for output
 
    fpIn = fopen("1.wav", "rb");
    fpOut = fopen("out1.aac", "wb");
 
    // (1) Open FAAC engine
    hEncoder = faacEncOpen(nSampleRate, nChannels, &nInputSamples, &nMaxOutputBytes);
    if(hEncoder == NULL)
    {
        printf("[ERROR] Failed to call faacEncOpen()\n");
        return -1;
    }
 
    nPCMBufferSize = nInputSamples * nPCMBitSize / 8;
    pbPCMBuffer = new BYTE [nPCMBufferSize];
    pbAACBuffer = new BYTE [nMaxOutputBytes];
 
    // (2.1) Get current encoding configuration
    pConfiguration = faacEncGetCurrentConfiguration(hEncoder);
    pConfiguration->inputFormat = FAAC_INPUT_16BIT;
    pConfiguration->aacObjectType=LOW;
 
    // (2.2) Set encoding configuration
    nRet = faacEncSetConfiguration(hEncoder, pConfiguration);
 
    for(int i = 0; 1; i++)
    {
        // 读入的实际字节数，最大不会超过nPCMBufferSize，一般只有读到文件尾时才不是这个值
        nBytesRead = fread(pbPCMBuffer, 1, nPCMBufferSize, fpIn);
 
        // 输入样本数，用实际读入字节数计算，一般只有读到文件尾时才不是nPCMBufferSize/(nPCMBitSize/8);
        nInputSamples = nBytesRead / (nPCMBitSize / 8);
         printf(".....[%d].....[%d].........[%d]\n",nBytesRead ,nInputSamples,nMaxOutputBytes); 
        // (3) Encode
        nRet = faacEncEncode(
        hEncoder, (int*) pbPCMBuffer, nInputSamples, pbAACBuffer, nMaxOutputBytes);
 
        fwrite(pbAACBuffer, 1, nRet, fpOut);
 
        printf("%d: faacEncEncode returns %d\n", i, nRet);
 
        if(nBytesRead <= 0)
        {
            break;
        }
    }
 
    /*
    while(1)
    {
        // (3) Flushing
        nRet = faacEncEncode(
        hEncoder, (int*) pbPCMBuffer, 0, pbAACBuffer, nMaxOutputBytes);
        if(nRet <= 0)
        {
            break;
        }
    }
    */
 
    // (4) Close FAAC engine
    nRet = faacEncClose(hEncoder);
 
    delete[] pbPCMBuffer;
    delete[] pbAACBuffer;
    fclose(fpIn);
    fclose(fpOut);
 
    //getchar();
 
    return 0;
}
