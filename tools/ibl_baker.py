import numpy as np
from PIL import Image
import sys
import os

def generate_brdf_lut(size=256):
    """Generate BRDF LUT (scale, bias for envBRDF)"""
    lut = np.zeros((size, size, 2))
    NdotV = np.linspace(0, 1, size)
    roughness = np.linspace(0, 1, size)
    
    N_V = np.stack([NdotV, roughness], axis=-1)
    
    for i in range(size):
        for j in range(size):
            ndv = N_V[i][j][0]
            rough = N_V[i][j][1]
            # Simplified split-sum approx
            F0 = 0.04
            scale = 1.0 - F0
            bias = F0
            lut[i][j] = [scale * rough * (1.0 - ndv), bias * (1.0 - rough)]
    
    img = Image.fromarray((lut * 255).astype(np.uint8), 'RGB')
    os.makedirs('../../assets/shaders/textshaders', exist_ok=True)
    img.save('../../assets/shaders/textshaders/brdfLUT.png')
    print("Generated brdfLUT.png")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python ibl_baker.py <hdr_file>")
        sys.exit(1)
    
    hdr_path = sys.argv[1]
    generate_brdf_lut()
    print("Note: Cubemaps require HDR loader (OpenEXR/imageio); download sunset HDR and extend.")
    # Todo: full IBL with imageio.imread(hdr) -> equirect -> cubemap conv.

