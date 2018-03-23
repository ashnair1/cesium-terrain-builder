/******************************************************************************
 *
 * Project:  GDAL Core
 * Purpose:  Implementation of a dataset overview warping class
 * Author:   Even Rouault, <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2014, Even Rouault, <even dot rouault at spatialys dot com>
 *
 * SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "cpl_port.h"
#include "gdal_priv.h"

#include <cstring>

#include "cpl_conv.h"
#include "cpl_error.h"
#include "cpl_progress.h"
#include "cpl_string.h"
#include "gdal.h"
#include "gdal_mdreader.h"
#include "gdal_proxy.h"

/** In GDAL, GDALRasterBand::GetOverview() returns a stand-alone band, that may
    have no parent dataset. This can be inconvenient in certain contexts, where
    cross-band processing must be done, or when API expect a fully fledged
    dataset.  Furthermore even if overview band has a container dataset, that
    one often fails to declare its projection, geotransform, etc... which make
    it somehow useless. GDALOverviewDataset remedies to those deficiencies.
*/

GDALDataset *GDALCreateOverviewDataset(GDALDataset *poMainDS, int nOvrLevel,
                                       bool bThisLevelOnly);
