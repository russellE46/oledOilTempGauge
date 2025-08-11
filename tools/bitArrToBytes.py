from pathlib import Path

SCRIPT_DIR = str(Path(__file__).resolve().parent)

IMAGE_WIDTH = 128               # bits

# ************************************************************************** 
# * @brief - byteArrToBits()
# * Takes the contents of outputs/bitArr.txt, turns them into a byte array, and outputs
# * the product to "byteArr.txt"
# * 
# * @return - None
# *************************************************************************
def bitArrToBytes():
    byteArr = []
    byte = 0

    with open(SCRIPT_DIR + "/outputs/bitArr.txt", 'r') as f:
        txt = f.read()
        txt = txt.replace('\n', '')
        for i in range(0, len(txt)):
            if ((i > 0) and 
                (i % 8 == 0)):
                byteArr.append(byte)
                byte = 0

            byteInd = 7 - (i % 8)
            byte += int(txt[i]) << byteInd
            
        byteArr.append(byte)

    print (len(byteArr))
    with open(SCRIPT_DIR + "/outputs/byteArr.txt", "w") as f:
        f.write('\t{\n\t\t')
        for i, byte in enumerate(byteArr, 0):
            if (byte < 0x10):
                hexStr = "0x" + "0" + hex(byte)[2]
            else:
                hexStr = hex(byte)

            f.write(hexStr)
            if (i  < (len(byteArr) - 1)):
                f.write(", ")
            if (i % (IMAGE_WIDTH / 8) == ((IMAGE_WIDTH / 8) - 1)):
                f.write('\n\t\t')
        f.write('\n\t};')


if (__name__ == "__main__"):
    bitArrToBytes()

