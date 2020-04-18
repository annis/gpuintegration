#ifndef Y3_CLUSTER_CPP_MOR_SDSS_HH
#define Y3_CLUSTER_CPP_MOR_SDSS_HH

#include "utils/datablock.hh"
#include "utils/datablock_reader.hh"
#include "utils/interp_2d.hh"
#include "utils/mz_power_law.hh"
#include "utils/primitives.hh"

#include <cmath>
#include <iomanip>
#include <istream>
#include <ostream>

namespace y3_cluster {

  class MOR_sdss {
    // Read from l_sat_grid.dat
    static std::array<double, 42> constexpr test_lsat = {
      0.500000,   0.600000,   0.700000,   0.800000,   0.900000,   1.000000,
      2.000000,   3.000000,   4.000000,   5.000000,   6.000000,   7.000000,
      8.000000,   9.000000,   10.000000,  11.000000,  12.000000,  13.000000,
      14.000000,  24.000000,  34.000000,  44.000000,  54.000000,  64.000000,
      74.000000,  84.000000,  94.000000,  104.000000, 114.000000, 124.000000,
      134.000000, 144.000000, 154.000000, 164.000000, 174.000000, 184.000000,
      194.000000, 204.000000, 214.000000, 224.000000, 234.000000, 244.000000};

    // Read from sig_intr_grid.dat
    static std::array<double, 14> constexpr test_sigintr = {0.050000,
                                                            0.100000,
                                                            0.200000,
                                                            0.300000,
                                                            0.400000,
                                                            0.500000,
                                                            0.600000,
                                                            0.700000,
                                                            0.800000,
                                                            0.900000,
                                                            1.000000,
                                                            1.300000,
                                                            1.700000,
                                                            2.000000};

    // Read from sig_skew_table.dat;
    static std::array<std::array<double, 42>,
                      14> constexpr sig_skewnorml_flat = {
      {{1.211319,  1.307980,  1.455547,  1.242204,  1.260416,  1.287859,
        1.604654,  1.883786,  2.133369,  2.357737,  2.565367,  2.759611,
        2.939715,  3.113878,  3.277425,  3.435430,  3.584226,  3.730049,
        3.872571,  5.091403,  6.118126,  7.024295,  7.861908,  8.647303,
        9.392576,  10.110787, 10.799733, 11.464516, 12.127780, 12.770096,
        13.393245, 14.018971, 14.625637, 15.228590, 15.823349, 16.405359,
        16.995068, 17.565897, 18.144194, 18.704318, 19.271479, 19.830625},
       {1.211227,  1.309620,  1.455854,  1.241931,  1.259493,  1.287663,
        1.605865,  1.898566,  2.160331,  2.397456,  2.621481,  2.828296,
        3.026425,  3.213295,  3.393675,  3.568839,  3.735769,  3.899172,
        4.061805,  5.508045,  6.791948,  7.997309,  9.155444,  10.276979,
        11.376409, 12.460638, 13.535322, 14.584471, 15.645481, 16.685029,
        17.736828, 18.756013, 19.791557, 20.837728, 21.859120, 22.878280,
        23.911779, 24.911220, 25.953940, 26.954718, 27.983187, 28.968243},
       {1.211671,  1.309826,  1.456078,  1.242446,  1.260993,  1.290490,
        1.647297,  1.975678,  2.278038,  2.560308,  2.830641,  3.086770,
        3.336938,  3.585123,  3.821795,  4.058084,  4.285488,  4.513090,
        4.741029,  6.911372,  9.004724,  11.059253, 13.091911, 15.109313,
        17.140012, 19.159858, 21.177185, 23.182035, 25.199804, 27.212651,
        29.216700, 31.205516, 33.209544, 35.219265, 37.216348, 39.230041,
        41.242532, 43.245237, 45.252177, 47.220149, 49.236398, 51.248283},
       {1.211565,  1.311895,  1.460779,  1.251060,  1.277766,  1.312986,
        1.716564,  2.098457,  2.459967,  2.805381,  3.143568,  3.475037,
        3.798101,  4.120511,  4.438261,  4.753240,  5.068811,  5.378748,
        5.691811,  8.755126,  11.787875, 14.802370, 17.808191, 20.817430,
        23.841264, 26.825406, 29.846444, 32.848399, 35.844798, 38.838005,
        41.835642, 44.807681, 47.874351, 50.846945, 53.843309, 56.831403,
        59.850411, 62.868051, 65.831786, 68.818846, 71.820014, 74.862193},
       {1.214203,  1.316602,  1.475982,  1.273027,  1.306867,  1.349644,
        1.807364,  2.254934,  2.688736,  3.113848,  3.533751,  3.945386,
        4.358664,  4.765982,  5.172834,  5.580771,  5.985243,  6.394095,
        6.800004,  10.816307, 14.825071, 18.828409, 22.833925, 26.835960,
        30.840218, 34.822678, 38.821903, 42.845150, 46.865463, 50.844324,
        54.834713, 58.883133, 62.837446, 66.850908, 70.830658, 74.871135,
        78.816211, 82.872843, 86.830251, 90.827108, 94.868737, 98.851579},
       {1.223224,  1.305364,   1.282591,   1.302511,   1.343813,   1.391583,
        1.915889,  2.438440,   2.954782,   3.463715,   3.972178,   4.475866,
        4.981030,  5.484776,   5.988101,   6.489089,   6.987325,   7.492044,
        7.992565,  13.002149,  18.004261,  23.007252,  27.992595,  32.997420,
        38.007604, 42.995294,  47.960506,  53.006768,  57.998229,  62.978091,
        67.990712, 73.025313,  77.988608,  82.992045,  87.948481,  93.020257,
        98.037134, 103.005688, 107.968164, 112.955753, 118.091016, 123.064706},
       {1.237303,   1.297838,   1.299703,   1.336018,   1.386345,   1.440389,
        2.038549,   2.643681,   3.246379,   3.847373,   4.449428,   5.049205,
        5.648647,   6.246004,   6.847078,   7.447189,   8.042671,   8.641741,
        9.246661,   15.240034,  21.245192,  27.235472,  33.234248,  39.222670,
        45.239112,  51.237171,  57.234173,  63.224968,  69.264088,  75.216487,
        81.220146,  87.247497,  93.207614,  99.264302,  105.290686, 111.221248,
        117.130732, 123.207067, 129.211764, 135.248975, 141.199774, 147.221067},
       {1.255781,   1.301398,   1.326024,   1.373377,   1.431998,   1.494301,
        2.170349,   2.863678,   3.556875,   4.254405,   4.951657,   5.648337,
        6.344185,   7.042540,   7.738373,   8.438037,   9.135133,   9.842018,
        10.534453,  17.527653,  24.518568,  31.522017,  38.530695,  45.531330,
        52.535863,  59.486804,  66.539248,  73.511258,  80.457846,  87.477414,
        94.477479,  101.512152, 108.536001, 115.476406, 122.473642, 129.513069,
        136.476210, 143.513249, 150.550130, 157.499933, 164.525677, 171.621905},
       {1.276646,   1.315435,   1.357289,   1.415508,   1.481102,   1.551490,
        2.313390,   3.097829,   3.885637,   4.678081,   5.472808,   6.266119,
        7.063419,   7.858818,   8.656639,   9.454800,   10.253293,  11.045166,
        11.850929,  19.840428,  27.836272,  35.842002,  43.822372,  51.811789,
        59.830339,  67.850225,  75.868274,  83.871893,  91.824629,  99.842206,
        107.837178, 115.747088, 123.801923, 131.879473, 139.904745, 147.825008,
        155.800340, 163.858451, 171.872588, 179.842903, 187.749487, 195.829631},
       {1.296820,   1.336911,   1.392021,   1.460964,   1.534856,   1.613478,
        2.460424,   3.337350,   4.224161,   5.113733,   6.005077,   6.902279,
        7.794092,   8.695950,   9.585411,   10.488258,  11.382026,  12.283516,
        13.176733,  22.163355,  31.175048,  40.148876,  49.169087,  58.188614,
        67.159212,  76.140945,  85.190444,  94.138865,  103.180710, 112.151019,
        121.169030, 130.155468, 139.123738, 148.199908, 157.191572, 166.110967,
        175.158204, 184.278367, 193.189233, 202.156366, 211.208459, 220.106192},
       {1.314956,   1.362826,   1.430397,   1.507732,   1.590281,   1.677336,
        2.615813,   3.587581,   4.569082,   5.561364,   6.549072,   7.547573,
        8.537343,   9.537660,   10.527682,  11.533997,  12.527410,  13.528436,
        14.527811,  24.523978,  34.511355,  44.495985,  54.522808,  64.523901,
        74.496739,  84.506243,  94.476834,  104.521732, 114.526516, 124.492931,
        134.507726, 144.512764, 154.469854, 164.565066, 174.511718, 184.505784,
        194.563024, 204.550301, 214.551573, 224.414200, 234.453502, 244.490757},
       {1.377952,   1.458798,   1.554977,   1.660817,   1.772187,   1.886016,
        3.108236,   4.371942,   5.654082,   6.943083,   8.236469,   9.529075,
        10.822614,  12.119244,  13.411976,  14.715629,  16.011403,  17.304457,
        18.609621,  31.606019,  44.629317,  57.606470,  70.570352,  83.623095,
        96.649602,  109.608422, 122.636289, 135.528202, 148.553542, 161.627864,
        174.639075, 187.644904, 200.648839, 213.598369, 226.691110, 239.567775,
        252.597781, 265.726680, 278.632619, 291.690736, 304.684948, 317.539261},
       {1.484981,   1.607807,   1.744091,   1.888228,   2.038241,   2.191676,
        3.805323,   5.474649,   7.158217,   8.843498,   10.533764,  12.229865,
        13.922527,  15.619515,  17.316468,  19.033038,  20.717065,  22.416868,
        24.117551,  41.117558,  58.087372,  75.122262,  92.070802,  109.109606,
        126.060604, 143.111422, 160.052720, 176.993527, 193.966219, 210.996823,
        228.048774, 244.927949, 262.217855, 279.071730, 295.979342, 313.206434,
        329.907372, 347.043429, 364.039406, 381.250446, 398.166340, 414.966776},
       {1.576173,   1.731248,   1.898551,   2.074172,   2.252728,
        2.436570,   4.353815,   6.322445,   8.313083,   10.294745,
        12.287411,  14.277241,  16.277280,  18.268876,  20.271681,
        22.276521,  24.263926,  26.265196,  28.259014,  48.256424,
        68.222320,  88.220667,  108.266808, 128.276115, 148.278550,
        168.292247, 188.270702, 208.207174, 228.281385, 248.326064,
        268.274824, 288.273748, 308.305930, 328.090806, 348.214194,
        368.297940, 388.180241, 408.444446, 428.281276, 448.256689,
        468.212324, 488.220129}}};

    // Read from skew_table.dat;
    static std::array<std::array<double, 42>, 14> constexpr skews = {
      {{9.120214, 11.475627, 38.412414, 2.221504, 1.684393, 1.446896, 0.781268,
        0.600871, 0.508428,  0.446559,  0.403048, 0.369343, 0.343748, 0.322870,
        0.304779, 0.289879,  0.276274,  0.265372, 0.255227, 0.191968, 0.160551,
        0.140669, 0.124834,  0.115103,  0.105681, 0.099399, 0.092551, 0.088156,
        0.082999, 0.080240,  0.076111,  0.073012, 0.071480, 0.068041, 0.065188,
        0.063611, 0.062579,  0.059057,  0.057525, 0.056310, 0.055544, 0.053913},
       {9.145098, 11.542642, 38.444059, 2.215191, 1.685763, 1.446827, 0.785224,
        0.600837, 0.504202,  0.441256,  0.399798, 0.366120, 0.340445, 0.319289,
        0.299703, 0.285555,  0.271096,  0.259854, 0.250297, 0.185038, 0.152457,
        0.130736, 0.115380,  0.103652,  0.095467, 0.087519, 0.081429, 0.075389,
        0.070460, 0.066910,  0.063430,  0.060366, 0.057555, 0.054277, 0.051492,
        0.050459, 0.048013,  0.045600,  0.044636, 0.042930, 0.040863, 0.040592},
       {9.130929, 11.496395, 38.553494, 2.213733, 1.686796, 1.444697, 0.773524,
        0.587195, 0.491120,  0.427417,  0.383929, 0.349077, 0.322633, 0.300579,
        0.282022, 0.265832,  0.250816,  0.239020, 0.228564, 0.160977, 0.124857,
        0.103811, 0.088318,  0.078253,  0.068526, 0.061949, 0.056645, 0.051888,
        0.047442, 0.044319,  0.042033,  0.038364, 0.036582, 0.034254, 0.032967,
        0.030693, 0.029447,  0.027500,  0.027629, 0.025326, 0.023565, 0.024128},
       {9.090881, 11.387960, 36.486987, 2.138348, 1.651012, 1.419899, 0.750984,
        0.567105, 0.468460,  0.404457,  0.358641, 0.324854, 0.296326, 0.273723,
        0.254571, 0.238595,  0.224091,  0.211870, 0.200157, 0.133642, 0.100628,
        0.080240, 0.067621,  0.057301,  0.050274, 0.045249, 0.040040, 0.036396,
        0.033308, 0.031502,  0.029914,  0.027198, 0.026498, 0.023856, 0.022186,
        0.022131, 0.021086,  0.019331,  0.018129, 0.017506, 0.017330, 0.015956},
       {8.495180, 10.051448, 30.876760, 1.985017, 1.581239, 1.371586, 0.722252,
        0.536916, 0.439019,  0.374528,  0.328893, 0.293841, 0.266643, 0.243633,
        0.224536, 0.209399,  0.195256,  0.183810, 0.174124, 0.110965, 0.081662,
        0.064387, 0.053649,  0.045021,  0.040079, 0.034299, 0.031512, 0.028912,
        0.025870, 0.023838,  0.022831,  0.021006, 0.020104, 0.018436, 0.016838,
        0.016731, 0.016235,  0.015160,  0.013796, 0.013969, 0.012440, 0.012756},
       {7.280436, 6.954720, 2.759347, 1.828208, 1.501224, 1.304616, 0.684929,
        0.504954, 0.406041, 0.342880, 0.299160, 0.264531, 0.237149, 0.217317,
        0.199300, 0.183890, 0.171015, 0.160512, 0.150267, 0.093537, 0.067263,
        0.054102, 0.044438, 0.037667, 0.032276, 0.028385, 0.025535, 0.023194,
        0.021282, 0.019795, 0.019279, 0.017462, 0.015565, 0.015138, 0.013917,
        0.013269, 0.013570, 0.012332, 0.011480, 0.010412, 0.009803, 0.009744},
       {6.044311, 4.464386, 2.270965, 1.683147, 1.408570, 1.231329, 0.647790,
        0.469020, 0.374580, 0.313057, 0.270302, 0.238628, 0.213000, 0.192970,
        0.176484, 0.162463, 0.150547, 0.139979, 0.130501, 0.079703, 0.057365,
        0.045737, 0.036707, 0.031797, 0.027383, 0.024354, 0.022312, 0.020679,
        0.017407, 0.016420, 0.014826, 0.014685, 0.013240, 0.012463, 0.012894,
        0.010440, 0.010327, 0.009738, 0.009333, 0.009624, 0.008553, 0.009300},
       {5.035575, 3.204568, 1.987774, 1.555399, 1.318778, 1.160449, 0.606904,
        0.436103, 0.343434, 0.287311, 0.246226, 0.215142, 0.192055, 0.171639,
        0.157195, 0.144339, 0.133287, 0.123859, 0.115028, 0.069635, 0.051185,
        0.039354, 0.031507, 0.026961, 0.023131, 0.020698, 0.019201, 0.017478,
        0.016249, 0.014521, 0.013224, 0.012654, 0.011854, 0.011207, 0.010067,
        0.010211, 0.008342, 0.008700, 0.008625, 0.007851, 0.006954, 0.007121},
       {4.237669, 2.575665, 1.795866, 1.443392, 1.233284, 1.092845, 0.570030,
        0.404245, 0.316789, 0.262220, 0.223466, 0.194478, 0.172904, 0.155156,
        0.141943, 0.130392, 0.120730, 0.111397, 0.102695, 0.063214, 0.045266,
        0.033946, 0.028248, 0.024592, 0.020644, 0.018409, 0.016099, 0.014638,
        0.013580, 0.012534, 0.011060, 0.010175, 0.009928, 0.009802, 0.009131,
        0.008032, 0.008216, 0.006900, 0.007915, 0.007030, 0.006235, 0.006205},
       {3.567612, 2.217888, 1.638238, 1.343229, 1.157161, 1.025149, 0.533331,
        0.375971, 0.293297, 0.240604, 0.204908, 0.178419, 0.157993, 0.141526,
        0.128039, 0.117845, 0.107844, 0.100090, 0.093206, 0.055927, 0.039632,
        0.031419, 0.025441, 0.022017, 0.017970, 0.017046, 0.013781, 0.013837,
        0.011788, 0.010844, 0.010719, 0.009848, 0.008781, 0.008666, 0.008390,
        0.007383, 0.006704, 0.006690, 0.006704, 0.005597, 0.005315, 0.006187},
       {3.025613, 1.971907, 1.508690, 1.253347, 1.085729, 0.964902, 0.500720,
        0.350103, 0.270796, 0.222378, 0.189370, 0.163883, 0.144245, 0.129899,
        0.117215, 0.106412, 0.098729, 0.091369, 0.084997, 0.050933, 0.035450,
        0.028079, 0.022752, 0.019002, 0.016494, 0.014958, 0.013209, 0.011917,
        0.010909, 0.011051, 0.009086, 0.008943, 0.008319, 0.007697, 0.007308,
        0.006254, 0.006289, 0.006687, 0.005169, 0.005813, 0.005401, 0.005271},
       {2.103528, 1.521551, 1.224627, 1.037122, 0.908020, 0.811763, 0.418322,
        0.287979, 0.220201, 0.178456, 0.150544, 0.130941, 0.114658, 0.101290,
        0.093054, 0.083796, 0.078241, 0.071278, 0.066739, 0.039318, 0.027836,
        0.021947, 0.017540, 0.015152, 0.012936, 0.011566, 0.010478, 0.008942,
        0.008706, 0.008533, 0.007591, 0.006950, 0.005415, 0.006408, 0.005202,
        0.005010, 0.005253, 0.005104, 0.004322, 0.005458, 0.004121, 0.004314},
       {1.536751, 1.182345, 0.975411, 0.837641, 0.736944, 0.660374, 0.337183,
        0.229715, 0.174118, 0.140625, 0.118463, 0.102568, 0.089017, 0.079632,
        0.072125, 0.066294, 0.059757, 0.055515, 0.051649, 0.030344, 0.020841,
        0.016917, 0.013317, 0.011891, 0.010315, 0.008493, 0.007762, 0.006624,
        0.007287, 0.006208, 0.005151, 0.005726, 0.004994, 0.004219, 0.004152,
        0.003915, 0.003437, 0.003389, 0.003397, 0.002747, 0.002961, 0.003400},
       {1.286577, 1.011802, 0.843325, 0.727349, 0.642881, 0.577038, 0.293274,
        0.199210, 0.150758, 0.121809, 0.101102, 0.087423, 0.077013, 0.067438,
        0.060997, 0.055508, 0.050853, 0.048553, 0.044361, 0.026014, 0.018513,
        0.014217, 0.011612, 0.009604, 0.007862, 0.007154, 0.005997, 0.006527,
        0.005605, 0.004194, 0.004040, 0.004002, 0.004299, 0.004607, 0.004030,
        0.003770, 0.002902, 0.003709, 0.002591, 0.001857, 0.002333, 0.002402}}};

    static y3_cluster::Interp2D const sig_interp;
    static y3_cluster::Interp2D const skews_interp;

    double _A = 0.0;
    double _B = 0.0;
    double _C = 0.0;
    double _sigma_intr = 0.0;

  public:
    MOR_sdss() = default;

    MOR_sdss(double A, double B, double C, double sigma_i)
      : _A(A), _B(B), _C(C), _sigma_intr(sigma_i)
    {}

    explicit MOR_sdss(cosmosis::DataBlock& sample)
      : _A(get_datablock<double>(sample, "cluster_abundance", "mor_A"))
      , _B(get_datablock<double>(sample, "cluster_abundance", "mor_B"))
      , _C(get_datablock<double>(sample, "cluster_abundance", "mor_alpha"))
      , _sigma_intr(
          get_datablock<double>(sample, "cluster_abundance", "mor_sigma"))
    {}

    double
    operator()(double lt, double lnM, double) const
    {
      // Now _lambda returns the evaluation of the eq.(9) of the
      // Matteo's paper, i.e., lambda_sat_given_M. 1. is a dummy
      // value for z. We are not using z here.
      double ltm = pow((std::exp(lnM) - _A) / (_B - _A), _C);

      // Computing sigma from the interpolation
      // ltm is lambda_true_given_M; _sigma_intr is sigma_intrisic
      double _sigma = sig_interp(_sigma_intr, ltm);
      double _skw = skews_interp(_sigma_intr, ltm);

      // Eq. B1 of Matteo's paper, adding the normalization part
      double const x = lt - ltm;
      double const erfarg = -1.0 * _skw * (x) / (std::sqrt(2.) * _sigma);
      double const erfterm = std::erfc(erfarg);
      return y3_cluster::gaussian(x, 0.0, _sigma) * erfterm;
    }

    friend std::ostream&
    operator<<(std::ostream& os, MOR_sdss const& m)
    {
      auto const old_flags = os.flags();
      os << std::hexfloat << m._A << ' ' << m._B << ' ' << m._C << ' '
         << m._sigma_intr;
      os.flags(old_flags);
      return os;
    }

    friend std::istream&
    operator>>(std::istream& is, MOR_sdss& m)
    {
      assert(is.good());
      std::string buffer;
      std::getline(is, buffer);
      std::vector<double> vals = cosmosis::str_to_doubles(buffer);
      if (vals.size() == 4)
      {
        m._A = vals[0];
        m._B = vals[1];
        m._C = vals[2];
        m._sigma_intr = vals[3];
      }
      else
      {
        is.setstate(std::ios_base::failbit);
      };
      return is;
    }
  };
}

#endif
