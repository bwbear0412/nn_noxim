/*
 * NN-Noxim - the NoC-based ANN Simulator
 *
 * (C) 2018 by National Sun Yat-sen University in Taiwan
 *
 * This file contains the implementation of loading NN model
 */

#include "NNModel.h"
#include <iomanip>
#include <math.h>
#include <ctime>
#include <string>

NNModel::NNModel() {
}

//M_fname Useless tytyty
bool NNModel::load() {
    cout << "model file loading (filename: " << NoximGlobalParams::NNmodel_filename << ")..." << endl;
    ifstream           fin(NoximGlobalParams::NNmodel_filename, ios::in);
    char               temp_type[20], temp_sv_pad[20], temp_actfun[10];
    int                temp;
    int                temp_c_x, temp_c_y, temp_z, temp_num, temp_std, temp_x, temp_y, temp_pad, temp_channels, temp_c_z;
    int                input_size, output_size;
    deque<deque<int> > conv;        // 卷积矩阵
    deque<deque<int> > pool;        // 池化矩阵
    int                all_Nue = 0;
    // *****************all layer Neu_num setting*******************
    cout << endl;
    cout << "layer_ID |    type | Neu_num |       X |       Y | channel |   C/P_X |   C/P_Y |    C/P_Z |  stride | padding | act_fun |" << endl;
    cout << "-------------------------------------------------------------------------------------------------------------------------" << endl;

    // model.txt 文件读取完毕
    // Input 32 32 1
    // Convolution 28 28 6 5 5 1 1 0 relu
    // Pooling 14 14 6 2 2 2 average
    // Convolution 10 10 16 5 5 6 1 0 relu
    // Pooling 5 5 16 2 2 2 average
    // Dense 120 relu
    // Dense 84 relu
    // Dense 10 softmax
    // Input        输入层
    // Convolution  卷积层
    // Pooling      池化层
    // Dense        全连接层

    // all_leyer_type   层 类型
    // all_leyer_size   层 参数
    // temp_leyer_size  暂存层的参数
    // 输入层参数    输入的HWC H W C
    // 卷积层参数    输入的HWC H W C    卷积核H W C  步长 填充大小      激活方法
    // 池化层参数    输入的HWC H W C    池化核H W    步长              池化方法
    // 连接层参数    拉平的             方法

    while(fin >> temp_type) {
        if(!strcmp(temp_type, "Dense")) {
            all_leyer_type.push_back('f');
            char line[256];
            fin.getline(line, sizeof(line) - 1);
            sscanf(line, "%d %s", &temp, temp_actfun);
            deque<int> temp_leyer_size;
            temp_leyer_size.push_back(temp);
            if(!strcmp(temp_actfun, "relu")) {
                temp_leyer_size.push_back(RELU);
            }
            else if(!strcmp(temp_actfun, "tanh")) {
                temp_leyer_size.push_back(TANH);
            }
            else if(!strcmp(temp_actfun, "sigmoid")) {
                temp_leyer_size.push_back(SIGMOID);
            }
            else if(!strcmp(temp_actfun, "softmax")) {
                temp_leyer_size.push_back(SOFTMAX);
            }

            all_leyer_size.push_back(temp_leyer_size);
            all_Nue += temp;
            cout << setw(8) << all_leyer_type.size() - 1 << " |"
                 << setw(8) << "Fully" << " |"
                 << setw(8) << temp_leyer_size[0] << " |"
                 << setw(10) << " |"
                 << setw(10) << " |"
                 << setw(10) << " |"
                 << setw(10) << " |"
                 << setw(10) << " |"
                 << setw(10) << " |"
                 << setw(10) << " |"
                 << setw(10) << " |"
                 << setw(8) << temp_actfun << " |" << endl;
        }
        else if(!strcmp(temp_type, "Input")) {
            all_leyer_type.push_back('i');
            char line[256];
            fin.getline(line, sizeof(line) - 1);
            sscanf(line, "%d %d %d", &temp_x, &temp_y, &temp_z);
            temp = temp_x * temp_y * temp_z;
            deque<int> temp_leyer_size;
            temp_leyer_size.push_back(temp);
            temp_leyer_size.push_back(temp_x);
            temp_leyer_size.push_back(temp_y);
            temp_leyer_size.push_back(temp_z);

            all_leyer_size.push_back(temp_leyer_size);
            //all_Nue+=temp;
            cout << setw(8) << all_leyer_type.size() - 1 << " |" << setw(8) << "Input" << " |" << setw(10) << " |"
                 << setw(8) << temp_leyer_size[1] << " |" << setw(8) << temp_leyer_size[2] << " |" << setw(8) << temp_leyer_size[3] << " |"
                 << setw(10) << " |" << setw(10) << " |" << setw(10) << " |"
                 << setw(10) << " |" << setw(10) << " |" << setw(10) << " |" << endl;
        }
        else if(!strcmp(temp_type, "Convolution")) {
            all_leyer_type.push_back('c');
            char line[256];
            fin.getline(line, sizeof(line) - 1);
            sscanf(line, "%d %d %d %d %d %d %d %d %s", &temp_x, &temp_y, &temp_channels, &temp_c_x, &temp_c_y, &temp_c_z, &temp_std, &temp_pad, temp_actfun);
            deque<int> temp_leyer_size;
            temp = temp_x * temp_y * temp_channels;
            temp_leyer_size.push_back(temp);            // The size of the convolution layer
            temp_leyer_size.push_back(temp_x);
            temp_leyer_size.push_back(temp_y);
            temp_leyer_size.push_back(temp_channels);
            temp_leyer_size.push_back(temp_c_x);
            temp_leyer_size.push_back(temp_c_y);
            temp_leyer_size.push_back(temp_c_z);
            temp_leyer_size.push_back(temp_std);
            temp_leyer_size.push_back(temp_pad);
            if(!strcmp(temp_actfun, "relu")) {
                temp_leyer_size.push_back(RELU);
            }
            else if(!strcmp(temp_actfun, "tanh")) {
                temp_leyer_size.push_back(TANH);
            }
            else if(!strcmp(temp_actfun, "sigmoid")) {
                temp_leyer_size.push_back(SIGMOID);
            }
            else if(!strcmp(temp_actfun, "softmax")) {
                temp_leyer_size.push_back(SOFTMAX);
            }
            all_leyer_size.push_back(temp_leyer_size);
            all_Nue += temp;
            cout << setw(8) << all_leyer_type.size() - 1 << " |" << setw(8) << "Convol" << " |" << setw(8) << temp_leyer_size[0] << " |"
                 << setw(8) << temp_leyer_size[1] << " |" << setw(8) << temp_leyer_size[2] << " |" << setw(8) << temp_leyer_size[3] << " |"
                 << setw(8) << temp_leyer_size[4] << " |" << setw(8) << temp_leyer_size[5] << " |" << setw(8) << temp_leyer_size[6] << " |"
                 << setw(8) << temp_leyer_size[7] << " |" << setw(8) << temp_leyer_size[8] << " |" << setw(8) << temp_actfun << " |" << endl;
        }
        else if(!strcmp(temp_type, "Pooling")) {
            all_leyer_type.push_back('p');
            char line[256];
            fin.getline(line, sizeof(line) - 1);
            sscanf(line, "%d %d %d %d %d %d %s", &temp_x, &temp_y, &temp_channels, &temp_c_x, &temp_c_y, &temp_std, temp_actfun);
            deque<int> temp_leyer_size;
            temp = temp_x * temp_y * temp_channels;
            temp_leyer_size.push_back(temp);        // The size of the pooling layer
            temp_leyer_size.push_back(temp_x);
            temp_leyer_size.push_back(temp_y);
            temp_leyer_size.push_back(temp_channels);
            temp_leyer_size.push_back(temp_c_x);
            temp_leyer_size.push_back(temp_c_y);
            temp_leyer_size.push_back(temp_std);

            if(!strcmp(temp_actfun, "average")) {
                temp_leyer_size.push_back(AVERAGE);
            }
            else if(!strcmp(temp_actfun, "maximum")) {
                temp_leyer_size.push_back(MAXIMUM);
            }

            all_leyer_size.push_back(temp_leyer_size);
            all_Nue += temp;
            cout << setw(8) << all_leyer_type.size() - 1 << " |" << setw(8) << "Pooling" << " |" << setw(8) << temp_leyer_size[0] << " |"
                 << setw(8) << temp_leyer_size[1] << " |" << setw(8) << temp_leyer_size[2] << " |" << setw(8) << temp_leyer_size[3] << " |"
                 << setw(8) << temp_leyer_size[4] << " |" << setw(8) << temp_leyer_size[5] << " |" << setw(10) << " |" << setw(8) << temp_leyer_size[6] << " |"
                 << setw(10) << " |" << setw(8) << temp_actfun << " |" << endl;
        }
        else if(!strcmp(temp_type, "%")) {
            char line[256];
            fin.getline(line, sizeof(line) - 1);
        }
        else {
            cout << "!!Error model format: " << temp_type << " !!" << endl;
            char line[256];
            fin.getline(line, sizeof(line) - 1);
        }
    }
    // for(int i = 0; i < 5; i++) {
    //     cout << all_leyer_type[i] << "-------";
    // }
    cout << "model all_leyer complete" << endl;
    cout << "all neu:" << all_Nue << endl;
    fin.close();

    input_size  = all_leyer_size.front()[0];
    output_size = all_leyer_size.back()[0];

    //******************mapping information prepare************************
    mapping_table.clear();
    mapping_table.assign(NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y, -1);

    cout << "ALgorithm: " << NoximGlobalParams::mapping_algorithm << endl;
    if(!strcmp(NoximGlobalParams::mapping_algorithm, "random")) {
        srand(time(NULL));
        for(int i = 0; i < mapping_table.size(); i++) {
            while(1) {
                int map_point = rand() % mapping_table.size();
                if(mapping_table[map_point] == -1) {
                    mapping_table[map_point] = i;
                    break;
                }
            }
        }
    }
    else if(!strcmp(NoximGlobalParams::mapping_algorithm, "dir_x")) {
        for(int i = 0; i < mapping_table.size(); i++) {    //dir_x mapping
            mapping_table[i] = i;
        }
    }
    else if(!strcmp(NoximGlobalParams::mapping_algorithm, "dir_y")) {
        for(int i = 0; i < mapping_table.size(); i++) {    //dir_y mapping
            mapping_table[i] = (i % NoximGlobalParams::mesh_dim_x) * NoximGlobalParams::mesh_dim_x + (i / NoximGlobalParams::mesh_dim_x);
        }
    }
        // 需要我去编写映射算法
    else if(!strcmp(NoximGlobalParams::mapping_algorithm, "table")) {
        ifstream fin_m(NoximGlobalParams::mapping_table_filename, ios::in);
        cout << "mapping file loading (filename: " << NoximGlobalParams::mapping_table_filename << ")..." << endl;
        while(!fin_m.eof()) {
            char line[256];
            fin_m.getline(line, sizeof(line) - 1);
            if(line[0] != '\0') {
                if(line[0] != '%') {
                    int ID_Group, ID_PE;
                    sscanf(line, "%d %d", &ID_Group, &ID_PE);
                    mapping_table[ID_Group] = ID_PE;
                }
            }
        }
    }
    else {
        cout << "Error mapping algorithm!!" << endl;
        exit(1);
    }
    /*------Debugging--------*/
    // cout << "Mapping Table" << endl;
    // for(int i = 0; i < mapping_table.size(); i++) {
    //     cout << mapping_table.at(i) << "--";
    // }
    // cout << endl;
    /*-----------------------*/
    cout << "Model mapping table: " << mapping_table.size() << endl;
    cout << "maping complete" << endl;

    // ******************temp_Group_table setting**********************
    int   temp_ID_Neu      = 0;
    int   temp_layer       = 1; // Layer id starts from layer 1 which is convolution layer
    int   temp_ID_In_layer = 0;
    int   temp_ID_Group    = 0;
    int   temp_ID_In_Group = 0;
    float temp_w;
    int   temp_ID_conv     = 0;
    int   temp_ID_pool     = -1;

    deque<deque<float>>        temp_conv_weight;
    deque<deque<deque<float>>> temp_conv_weight_layer;
    int                        temp_layer_maxID = all_leyer_size[temp_layer][0]; // Starting with layer 1
    // cout << "Max id: " << temp_layer_maxID << endl;
    Group_table.clear();
    deque<NeuInformation> temp_Group_table;
    temp_Group_table.clear();
    deque<int> temp_leyer_ID_Group;
    temp_leyer_ID_Group.clear();

    // int          prevLayer_size;
    int          kernel_size;
    deque<float> temp_bias;
    ifstream     fin_w(NoximGlobalParams::NNweight_filename, ios::in);
    // 读取weight和bias
    cout << "weight file loading (filename: " << NoximGlobalParams::NNweight_filename << ")..." << endl;

    // Save convolution weights and bias for each filter
    deque<float> temp_weights;
    deque<float> temp_bias_conv;

    // 将weight.txt文件中的数据读进来
    for(int i = 0; i < all_leyer_type.size(); i++) {
        if(all_leyer_type[i] == 'c') {
            // 卷积层参数    输入的HWC H W C    卷积核H W C  步长 填充大小      激活方法
            // 卷积核大小    H * W
            kernel_size = all_leyer_size[i][4] * all_leyer_size[i][5];

            temp_conv_weight.clear();
            temp_bias_conv.clear();
            temp_conv_weight_layer.clear();
            // 输入通道数 C
            for(int j = 0; j < all_leyer_size[i][3]; j++) {
                for(int q = 0; q < all_leyer_size[i][6]; q++) {
                    temp_weights.clear();
                    for(int l = 0; l < kernel_size; l++) {
                        fin_w >> temp_w;
                        temp_weights.push_back(temp_w);
                    }
                    // Convolution kernel weights
                    temp_conv_weight.push_back(temp_weights);
                }
                temp_conv_weight_layer.push_back(temp_conv_weight);
                temp_conv_weight.clear();
            }
            all_conv_weight.push_back(temp_conv_weight_layer);
            temp_conv_weight_layer.clear();
            for(int m = 0; m < all_leyer_size[i][3]; m++) {
                fin_w >> temp_w;
                temp_bias_conv.push_back(temp_w);   // Bias for each filter
            }
            all_conv_bias.push_back(temp_bias_conv);
        }
    }

    /*--------------Debugging---------------------------*/
    // cout << all_conv_bias.size() << "--" << all_conv_bias[1].size() << endl;
    // for(int p = 0; p < all_conv_bias[1].size(); p++) {
    //     cout << all_conv_bias[1][p] << "-----";
    // }
    // cout << endl;
    // cout << all_conv_weight.size() << endl;
    //
    // cout << all_conv_weight[0].size() << endl;
    // cout << all_conv_weight[1].size() << endl;
    // cout << all_conv_weight[0][0].size() << endl;
    // cout << all_conv_weight[1][0].size() << endl;
    // cout << all_conv_weight[0][0][0].size() << endl;
    // cout << all_conv_weight[1][0][0].size() << endl;
    // for(int p = 0; p < all_conv_weight[1][0].size(); p++) {
    //     for(int q = 0; q < all_conv_weight[1][0][p].size(); q++) {
    //         cout << all_conv_weight[1][0][p][q] << "---";
    //     }
    //
    // }
    // cout << endl;
    /*--------------------------------------------------*/

    // NeuInformation 信息
    while(1) {
        NeuInformation NeuInfo;
        NeuInfo.ID_Neu = temp_ID_Neu;
        if(temp_ID_Neu < temp_layer_maxID) {
            NeuInfo.ID_layer    = temp_layer;
            NeuInfo.ID_In_layer = temp_ID_In_layer;
            if(all_leyer_type[temp_layer] == 'c') {
                NeuInfo.ID_conv = temp_ID_conv;
                // cout << "(" << temp_ID_Neu << "--" << NeuInfo.ID_conv << ")--";
            }
            else if(all_leyer_type[temp_layer] == 'p') {
                NeuInfo.ID_pool = temp_ID_pool;
                // cout << "(" << temp_ID_Neu << "--" << NeuInfo.ID_pool << ")--";
            }

            if(temp_ID_In_Group >= NoximGlobalParams::group_neu_num) {
                Group_table.push_back(temp_Group_table);
                temp_Group_table.clear();
                temp_ID_In_Group = 0;
                temp_leyer_ID_Group.push_back(temp_ID_Group);
                temp_ID_Group++;
            }

            NeuInfo.ID_In_Group = temp_ID_In_Group;
            NeuInfo.ID_Group    = temp_ID_Group;
            /*----------------Debugging-----------------*/
            // if(temp_layer == 1 || temp_layer == 3) {
            //     cout << "ID Conv : " << temp_ID_conv << "--";
            // }
            // else if(temp_layer == 2 || temp_layer == 4) {
            //     cout << "ID_pool: " << temp_ID_pool << "--";
            // }
            /*------------------------------------------*/
        }
        else {
            // change layer and group
            // cout << "Temp id in layer: " << temp_ID_In_layer << endl;
            Group_table.push_back(temp_Group_table);
            temp_Group_table.clear();

            NeuInfo.ID_layer = ++temp_layer;
            temp_leyer_ID_Group.push_back(temp_ID_Group);
            all_leyer_ID_Group.push_back(temp_leyer_ID_Group);
            temp_leyer_ID_Group.clear();
            NeuInfo.ID_Group = ++temp_ID_Group;

            if(temp_ID_Neu >= all_Nue) {
                break;
            }

            if(temp_ID_Group >= NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y) {
                cout << "error group_size or NoC_size" << endl;
            } // output error!!

            temp_ID_In_layer = 0;
            temp_ID_In_Group = 0;
            NeuInfo.ID_In_layer = temp_ID_In_layer;
            NeuInfo.ID_In_Group = temp_ID_In_Group;
            temp_layer_maxID += all_leyer_size[temp_layer][0];

            if(all_leyer_type[temp_layer] == 'c') {
                NeuInfo.ID_conv = ++temp_ID_conv;
                // cout << "(" << temp_ID_Neu << "--" << NeuInfo.ID_conv << ")--";

            }
            else if(all_leyer_type[temp_layer] == 'p') {
                NeuInfo.ID_pool = ++temp_ID_pool;
                // cout << "(" << temp_ID_Neu << "--" << NeuInfo.ID_pool << ")--";
            }
            temp_bias.clear();
            if(all_leyer_type[temp_layer] == 'f') {
                for(int k = 0; k < all_leyer_size[temp_layer][0]; k++) {
                    fin_w >> temp_w;
                    temp_bias.push_back(temp_w);
                }
                /*-------------Debugging------------*/
                // cout << temp_bias.back() << endl;
                // cout << temp_bias.front() << endl;
                // cout << "Size of temp_bias: " << temp_bias.size() << "--" << temp_layer << endl;
                /*----------------------------------*/
            }
        }

        NeuInfo.Type_layer = all_leyer_type[NeuInfo.ID_layer];
        if(NeuInfo.Type_layer == 'f') {
            for(int i = 0; i < (all_leyer_size[temp_layer - 1][0]); i++) {
                fin_w >> temp_w;
                NeuInfo.weight.push_back(temp_w);
            }
            NeuInfo.weight.push_back(temp_bias[NeuInfo.ID_In_layer]); //Include bias
            /*------Debugging------*/
            // cout << NeuInfo.weight.size() << "-" << NeuInfo.weight.front() << "-" << NeuInfo.weight.back() << endl;
            /*---------------------*/
        }
        // cout << endl;
        //***********mapping****************
        // all_conv_weight.push_back(temp_conv_weight);
        temp_Group_table.push_back(NeuInfo);
        temp_ID_In_Group++;
        temp_ID_In_layer++;
        temp_ID_Neu++;
    }
    // cout << endl << "Convolution weight: " << endl;
    // for(int i = 0; i < all_leyer_type.size(); i++) {
    //     if(all_leyer_type[i] == 'c') {
    //         for(int j = 0; j < (all_leyer_size[i][3] * all_leyer_size[i][4] * all_leyer_size[i][5] + all_leyer_size[i][3]); j++) {
    //             fin_w >> temp_w;
    //             temp_conv_weight.push_back(temp_w); //Convolution weight and then bias value
    //             cout << temp_w << "----";
    //         }
    //     }
    //     all_conv_weight.push_back(temp_conv_weight);
    // }
    fin_w.close();

    // deque<float>().swap(temp_conv_weight);
    deque<NeuInformation>().swap(temp_Group_table);
    deque<int>().swap(temp_leyer_ID_Group);
    cout << "model & group complete" << endl;


    /*-------------------Debugging---------------------*/
    // cout << Group_table[1][0].Type_layer << endl;
    // cout << Group_table[50][0].Type_layer << endl;;
    //
    // cout << "Group Table" << endl;
    // cout << "Neuron ID" << endl;
    // for(int i = 0; i < Group_table.size(); i++) {
    //     for(int j = 0; j < Group_table[i].size(); j++) {
    //         cout << Group_table[i][j].ID_Neu << "--";
    //
    //     }
    // }
    // cout << endl;
    //
    // cout << "Id in Group" << endl;
    // for(int i = 0; i < Group_table.size(); i++) {
    //     for(int j = 0; j < Group_table[i].size(); j++) {
    //         cout << Group_table[i][j].ID_In_Group << "--";
    //
    //     }
    // }
    // cout << endl;
    // cout << "Id in layer" << endl;
    // for(int i = 0; i < Group_table.size(); i++) {
    //     for(int j = 0; j < Group_table[i].size(); j++) {
    //         cout << Group_table[i][j].ID_In_layer << "--";
    //
    //     }
    // }
    // cout << endl;
    //
    // // Reverse Eng
    // cout << "All Layer ID Group" << endl;
    //
    //
    // for(int i = 0; i < all_leyer_ID_Group.size(); i++) {
    //     for(int j = 0; j < all_leyer_ID_Group[i].size(); j++) {
    //         cout << all_leyer_ID_Group[i][j] << "--";
    //
    //     }
    //
    // }
    //
    // cout << "All layer id group size: " << all_leyer_ID_Group.size() << endl;
    // // cout << "Last layer: " << all_leyer_ID_Group[6].size() << endl;
    // // cout << "First layer: " << all_leyer_ID_Group[0].size() << endl;
    // cout << "Group table size: " << Group_table.size() << endl;
    // cout << "all layer type size: " << all_leyer_type.size() << endl;
    // cout << "all layer size: " << all_leyer_size.size() << endl;
    //
    // cout << endl;
    // // Reverse Eng
    // cout << "All layer Size" << endl;
    // for(int i = 0; i < all_leyer_size.size(); i++) {
    //     for(int j = 0; j < all_leyer_size[i].size(); j++) {
    //         cout << all_leyer_size[i][j] << "--";
    //     }
    // }
    // cout << endl;
    //
    // // Reverse Eng
    // cout << "All layer Type" << endl;
    //
    // for(int i = 0; i < all_leyer_type.size(); i++) {
    //     cout << all_leyer_type[i] << "--";
    // }
    // cout << endl;
    /*-----------------------------------*/

    // 打印映射结果
    /******************print floorplan****************/
    cout << "Hardware floorplan:" << endl;
    cout << "  ";
    for(int i = 0; i < NoximGlobalParams::mesh_dim_x; i++) {
        cout << "-----";
    }
    cout << "-" << endl;
    for(int j = 0; j < NoximGlobalParams::mesh_dim_y; j++) {
        cout << "  |";
        for(int i = 0; i < NoximGlobalParams::mesh_dim_x; i++) {
            int loacl_id = j * NoximGlobalParams::mesh_dim_x + i;
            int x;
            for(x = 0; x < mapping_table.size(); x++) {
                if(mapping_table[x] == loacl_id) {
                    break;
                }
            }
            if(x < Group_table.size()) {
                int temp_lay = Group_table[x][0].ID_layer;
                cout << setw(3) << temp_lay << " |";
            }
            else {
                cout << setw(3) << " " << " |";
            }
        }
        cout << endl << "  ";
        for(int i = 0; i < NoximGlobalParams::mesh_dim_x; i++) {
            cout << "-----";
        }
        cout << "-" << endl;
    }

    // 数据输入
    //***********input setting***************
    fstream      fin_in(NoximGlobalParams::NNinput_filename, ios::in);
    float        temp_in;
    int          i = -1;
    deque<float> temp_data_in;
    temp_data_in.clear();
    while(fin_in >> temp_in) {
        i++;
        temp_data_in.push_back(temp_in);
        if(i == input_size - 1) {
            all_data_in.push_back(temp_data_in);
            temp_data_in.clear();
            i = -1;
        }
    }
    deque<float>().swap(temp_data_in);
    fin_in.close();

    cout << "all_data_in.size(): " << all_data_in.size() << endl;
    cout << "load input complete" << endl;
    /*--------Debugging-----------------*/
    // cout << "All data in" << endl;
    // for(int i = 0; i < all_data_in.size(); i++) {
    //     for(int j = 0; j < all_data_in[i].size(); j++) {
    //         cout << "( " << j << ": " << all_data_in[i][j] << ")--";
    //     }
    // }
    // cout << endl;
    // cout << "Size of input: " << all_data_in[0].size() << endl;
    /*----------------------------------*/

    //******************Save the coordinates for 2d Convolution and Pooling****************
    all_conv_coord.clear();
    all_pool_coord.clear();
    deque<int>        temp_cell;
    deque<deque<int>> temp_matrix;
    for(int           ab = 0; ab < all_leyer_size.size(); ab++) {
        if(all_leyer_type[ab] == 'c') {
            int coord_x     = all_leyer_size[ab][1];
            int coord_y     = all_leyer_size[ab][2];
            int coordPrev_x = all_leyer_size[ab - 1][1];
            int coordPrev_y = all_leyer_size[ab - 1][2];
            int kernel_x    = all_leyer_size[ab][4];
            int kernel_y    = all_leyer_size[ab][5];
            int kernel_z    = all_leyer_size[ab][6];
            int padding     = all_leyer_size[ab][8];

            if(padding == 0) {
                for(int aa = 0; aa < coord_x; aa++) {
                    for(int bb = 0; bb < coord_y; bb++) {
                        for(int cc = 0; cc < kernel_z; cc++) {
                            for(int dd = 0; dd < kernel_x; dd++) {
                                for(int ee = 0; ee < kernel_y; ee++) {
                                    temp_cell.push_back((aa + dd) * coordPrev_y + (ee + bb) + cc * coordPrev_x * coordPrev_y);
                                }
                            }
                        }
                        temp_matrix.push_back(temp_cell);
                        temp_cell.clear();
                    }
                }
                all_conv_coord.push_back(temp_matrix);
                temp_matrix.clear();
            }
            else if(padding == 1) {}

        }
        else if(all_leyer_type[ab] == 'p') {
            int coord_x     = all_leyer_size[ab][1];
            int coord_y     = all_leyer_size[ab][2];
            int coordPrev_x = all_leyer_size[ab - 1][1];
            int coordPrev_y = all_leyer_size[ab - 1][2];
            int kernel_x    = all_leyer_size[ab][4];
            int kernel_y    = all_leyer_size[ab][5];
            int stride      = all_leyer_size[ab][6];
            int horizontal  = 0;
            int vertical    = 0;
            temp_matrix.clear();
            temp_cell.clear();

            for(int aa = 0; aa < coord_x; aa++) {
                for(int bb = 0; bb < coord_y; bb++) {
                    if(aa > 0) { vertical = 1; }
                    if(bb > 0) { horizontal = 1; }
                    if(horizontal == 0 && vertical == 0) //0,0
                    {
                        for(int cc = 0; cc < kernel_x; cc++) {
                            for(int dd = 0; dd < kernel_y; dd++) {
                                temp_cell.push_back((aa + cc) * coordPrev_y + (bb + dd));
                            }
                        }

                    }
                    else if(horizontal == 0 && vertical == 1) {
                        for(int cc = 0; cc < kernel_x; cc++) {
                            for(int dd = 0; dd < kernel_y; dd++) {
                                temp_cell.push_back((aa * stride + cc) * coordPrev_y + (bb + dd));
                            }
                        }
                    }
                    else if(horizontal == 1 && vertical == 0) {
                        for(int cc = 0; cc < kernel_x; cc++) {
                            for(int dd = 0; dd < kernel_y; dd++) {
                                temp_cell.push_back((aa + cc) * coordPrev_y + (bb * stride + dd));
                            }
                        }
                    }
                    else if(horizontal == 1 && vertical == 1) {
                        for(int cc = 0; cc < kernel_x; cc++) {
                            for(int dd = 0; dd < kernel_y; dd++) {
                                temp_cell.push_back((aa * stride + cc) * coordPrev_y + (bb * stride + dd));
                            }
                        }
                    }
                    horizontal = 0;
                    vertical   = 0;
                    temp_matrix.push_back(temp_cell);
                    temp_cell.clear();
                }
            }
            all_pool_coord.push_back(temp_matrix);
            temp_matrix.clear();
        }
    }

    cout << "Convolution and Pooling layer's related activities are completed." << endl;
    /*--------------------Debugging-----------------------*/
    // cout << "Conv deque Size: " << all_conv_coord.size() << " Size zero: " << all_conv_coord[0].size() << "Size One: " << all_conv_coord[1].size() << endl;
    // for(int gg = 0; gg < all_conv_coord[1][0].size(); gg++) {
    //     cout << all_conv_coord[1][0][gg] << "--";
    // }
    // cout << all_conv_coord[1][0].size() << endl;
    // cout << "Pool deque Size: " << all_pool_coord.size() << " Size zero: " << all_pool_coord[0].size() << "Size One: " << all_pool_coord[1].size() << endl;
    // for(int gg = 0; gg < all_pool_coord[1][0].size(); gg++) {
    //     cout << all_pool_coord[1][0][gg] << "--";
    // }
    // cout << "All pool Zero: " << all_pool_coord[0][0][0] << endl;
    // cout << "size: " << all_pool_coord[0][0].size() << endl;
    /*----------------------------------------------------*/
    return true;
}



