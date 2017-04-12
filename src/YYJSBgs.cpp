#include "YYJSBgs.h"
#include "bgslibrary.h"


YYJSBgs::YYJSBgs(void)
{
    bgs = NULL;
}


YYJSBgs::~YYJSBgs(void)
{
}

/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
void YYJSBgs::createBgs(BgsNameEnum bgsName){


	if(bgs != NULL){
		delete bgs;
		bgs = NULL;
	}

    switch (bgsName)
    {
    case FrameDifferenceBGS:
        bgs = new FrameDifference();
        break;
    case StaticFrameDifferenceBGS:
        bgs = new StaticFrameDifference();
        break;
    case WeightedMovingMeanBGS:
        bgs = new WeightedMovingMean();
        break;
    case WeightedMovingVarianceBGS:
        bgs = new WeightedMovingVariance();
        break;
    case MixtureOfGaussianV1BGS:
        //
        break;
    case MixtureOfGaussianV2BGS:
        bgs = new MixtureOfGaussianV2();
        break;
    case BgsNameEnum::AdaptiveBackgroundLearning:
        bgs = new bgslibrary::algorithms::AdaptiveBackgroundLearning();
        break;
    case BgsNameEnum::AdaptiveSelectiveBackgroundLearning:
        bgs = new bgslibrary::algorithms::AdaptiveSelectiveBackgroundLearning();
        break;
    case GMG:
        //bgs = new GMG();
        break;
    case DPAdaptiveMedianBGS:
        bgs = new DPAdaptiveMedian();
        break;
    case DPGrimsonGMMBGS:
        bgs = new DPGrimsonGMM();
        break;
    case DPZivkovicAGMMBGS:
        bgs = new DPZivkovicAGMM();
        break;
    case DPMeanBGS:
        bgs = new DPMean();
        break;
    case DPWrenGABGS:
        bgs = new DPWrenGA();
        break;
    case DPPratiMediodBGS:
        bgs = new DPPratiMediod();
        break;
    case DPEigenbackgroundBGS:
        bgs = new DPEigenbackground();
        break;
    case DPTextureBGS:
        bgs = new DPTexture();
        break;
    case BgsNameEnum::T2FGMM_UM:
        bgs = new bgslibrary::algorithms::T2FGMM_UM();
        break;
    case BgsNameEnum::T2FGMM_UV:
        bgs = new bgslibrary::algorithms::T2FGMM_UV();
        break;
    case BgsNameEnum::T2FMRF_UM:
        bgs = new bgslibrary::algorithms::T2FMRF_UM();
        break;
    case BgsNameEnum::T2FMRF_UV:
        bgs = new bgslibrary::algorithms::T2FMRF_UV();
        break;
    case BgsNameEnum::FuzzySugenoIntegral:
        bgs = new bgslibrary::algorithms::FuzzySugenoIntegral();
        break;
    case BgsNameEnum::FuzzyChoquetIntegral:
        bgs = new bgslibrary::algorithms::FuzzyChoquetIntegral();
        break;
    case BgsNameEnum::LBSimpleGaussian:
        bgs = new bgslibrary::algorithms::LBSimpleGaussian();
        break;
    case BgsNameEnum::LBFuzzyGaussian:
        bgs = new bgslibrary::algorithms::LBFuzzyGaussian();
        break;
    case BgsNameEnum::LBMixtureOfGaussians:
        bgs = new bgslibrary::algorithms::LBMixtureOfGaussians();
        break;
    case BgsNameEnum::LBAdaptiveSOM:
        bgs = new bgslibrary::algorithms::LBAdaptiveSOM();
        break;
    case BgsNameEnum::LBFuzzyAdaptiveSOM:
        bgs = new bgslibrary::algorithms::LBFuzzyAdaptiveSOM();
        break;
    case MultiLayerBGS:
        bgs = new MultiLayer();
        break;
    case BgsNameEnum::VuMeter:
        bgs = new bgslibrary::algorithms::VuMeter();
        break;
    case BgsNameEnum::KDE:
        bgs = new bgslibrary::algorithms::KDE();
        break;
    case IndependentMultimodalBGS:
        bgs = new IndependentMultimodal();
        break;
    case SJN_MultiCueBGS:
        //bgs = new SJN_MultiCue();
        break;
    case SigmaDeltaBGS:
        bgs = new SigmaDelta();
        break;
    case SuBSENSEBGS:
        bgs = new SuBSENSE();
        break;
    case LOBSTERBGS:
        bgs = new LOBSTER();
        break;
    default:
        bgs = new DPAdaptiveMedian();
        break;
    }
    if(bgs != NULL)
        bgs->setShowOutput(false);
}

void YYJSBgs::process(Mat input,Mat& foreground,Mat& background){
    bgs->process(input,foreground,background);
        ;
}
