#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <limits>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using cv::Mat;

bool loadImage(const string& path, Mat& image) {
    
    bool ret = true;
    
    image = cv::imread(path, cv::IMREAD_COLOR);
    
    if (image.empty()) {
        std::cerr << "Erro ao carregar a imagem: " << path << endl;
        ret = false;
    }

    return ret;
}

Mat toGray(const Mat& image) {
    Mat gray_image;
    cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
    return gray_image;
}

void printImageInfo(const Mat& image) {
    cout << "Largura : " << image.cols << " px" << endl;
    cout << "Altura   : " << image.rows << " px" << endl;
    cout << "Canais   : " << image.channels() << endl;
}

Mat quantization(const Mat& gray_image, int levels) {
    CV_Assert(gray_image.type() == CV_8UC1);
    CV_Assert(levels >= 2);

    Mat quantized_image(gray_image.size(), CV_8UC1);

    for (int y = 0; y < gray_image.rows; ++y) {
        const uchar* src = gray_image.ptr<uchar>(y);
        uchar* dst = quantized_image.ptr<uchar>(y);

        for (int x = 0; x < gray_image.cols; ++x) {
            int v = src[x];

            int level_idx = static_cast<int>(std::lround((v * (levels - 1)) / 255.0));
            int map_value = static_cast<int>(std::lround((level_idx * 255.0) / (levels - 1)));

            dst[x] = static_cast<uchar>(cv::saturate_cast<uchar>(map_value));
        }
    }

    return quantized_image;
}

Mat binarization(const Mat& gray_image, int threshold) {
    CV_Assert(gray_image.type() == CV_8UC1);

    Mat binary_image(gray_image.size(), CV_8UC1);

    for (int y = 0; y < gray_image.rows; ++y) {
        const uchar* src = gray_image.ptr<uchar>(y);
        uchar* dst = binary_image.ptr<uchar>(y);

        for (int x = 0; x < gray_image.cols; ++x) {
            dst[x] = (src[x] >= threshold) ? 255 : 0;
        }
    }

    return binary_image;
}

Mat reducing(const Mat& gray_image) {
    CV_Assert(gray_image.type() == CV_8UC1);

    cv::Size halfSize(gray_image.cols / 2, gray_image.rows / 2);
    Mat reduced_image;

    cv::resize(gray_image, reduced_image, halfSize, 0, 0, cv::INTER_AREA);

    return reduced_image;
}

Mat restoreSize(const Mat& reduced_image, const cv::Size& original_size) {

    Mat restored_image;
    // Ampliação com interpolação bilinear.
    cv::resize(reduced_image, restored_image, original_size, 0, 0, cv::INTER_LINEAR);

    return restored_image;
}

void exercise1(const string& imagePath) {
    Mat image;
    if (!loadImage(imagePath, image)) return;

    cout << "\n[Exercicio 1] Informacoes da imagem colorida:\n";
    printImageInfo(image);

    cv::imshow("Imagem Original", image);
    
    cv::waitKey(0);
}


void exercise2(const string& imagePath) {
    Mat image;
    if (!loadImage(imagePath, image)) return;

    Mat gray_image = toGray(image);

    Mat comparison;
    cv::hconcat(image, gray_image, comparison);
    cv::namedWindow("Comparativo Original x Cinza", cv::WINDOW_NORMAL);
    cv::imshow("Comparativo Original x Cinza", comparison);
    cv::waitKey(0);
}

void exercise3(const string& imagePath) {
    Mat image;
    if (!loadImage(imagePath, image)) return;

    Mat gray_image = toGray(image);

    int x, y;
    cout << "\n[Exercicio 3] Informe as coordenadas do pixel (x y): ";
    cin >> x >> y;

    if (x < 0 || x >= gray_image.cols || y < 0 || y >= gray_image.rows) {
        std::cerr << "Coordenadas invalidas.\n";
        return;
    }

    int intensity = static_cast<int>(gray_image.at<uchar>(y, x));

    cout << "Intensidade no pixel (" << x << ", " << y << ") = " << intensity << endl;
}

void exercise4(const string& imagePath) {
    Mat image;
    if (!loadImage(imagePath, image)) return;

    Mat gray_image = toGray(image);

    Mat negative_image = 255 - gray_image;

    Mat comparison;
    cv::hconcat(gray_image, negative_image, comparison);
    cv::namedWindow("Comparativo de Negativo", cv::WINDOW_NORMAL);
    cv::imshow("Comparativo de Negativo", comparison);

    cv::waitKey(0);
}


void exercise5(const string& imagePath) {
    Mat image;
    if (!loadImage(imagePath, image)) return;

    Mat gray_image = toGray(image);

    int threshold = 0;

    cout << "\n[Exercicio 5] Informe o valor do limiar: ";
    cin >> threshold;

    Mat binary_image = binarization(gray_image, threshold);

    Mat comparison;
    cv::hconcat(gray_image, binary_image, comparison);
    cv::namedWindow("Comparativo de Binarizacao", cv::WINDOW_NORMAL);
    cv::imshow("Comparativo de Binarizacao", comparison);

    cv::waitKey(0);
}

void exercise6(const string& imagePath) {
    Mat image;
    if (!loadImage(imagePath, image)) return;

    Mat gray_image = toGray(image);

    Mat q128 = quantization(gray_image, 128);
    Mat q64  = quantization(gray_image, 64);
    Mat q16  = quantization(gray_image, 16);
    Mat q4   = quantization(gray_image, 4);
    
    cv::putText(q128, "Original", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2);
    cv::putText(q64, "64 Niveis", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2);
    cv::putText(q16, "16 Niveis", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2);
    cv::putText(q4, "4 Niveis", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2);

    Mat top_row, bottom_row, mosaic;
    cv::hconcat(q128, q64, top_row);
    cv::hconcat(q16, q4, bottom_row);
    cv::vconcat(top_row, bottom_row, mosaic);

    cv::namedWindow("Comparativo de Quantizacao", cv::WINDOW_NORMAL);
    cv::imshow("Comparativo de Quantizacao", mosaic);

    cout << "\nComparacao visual esperada:\n";
    cout << "- 128 niveis: perda quase imperceptivel.\n";
    cout << "- 64 niveis: discretizacao leve.\n";
    cout << "- 16 niveis: bandas de cinza mais visiveis.\n";
    cout << "- 4 niveis: imagem bastante posterizada.\n";

    cv::waitKey(0);
}

void exercise7(const string& imagePath) {
    Mat image;
    if (!loadImage(imagePath, image)) return;

    Mat gray_image = toGray(image);

    Mat reduced_image = reducing(gray_image);
    Mat restored_image = restoreSize(reduced_image, gray_image.size());

    // Redimensionar a imagem reduzida para o tamanho original para exibição lado a lado
    Mat display_reduced;
    cv::resize(reduced_image, display_reduced, gray_image.size(), 0, 0, cv::INTER_NEAREST);

    cv::putText(gray_image, "Original", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2);
    cv::putText(display_reduced, "Reduzida", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2);
    cv::putText(restored_image, "Reampliada", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2);

    Mat comparison;
    cv::hconcat(std::vector<Mat>{gray_image, display_reduced, restored_image}, comparison);

    cv::namedWindow("Reducao de resolucao espacial - Comparativo", cv::WINDOW_NORMAL);
    cv::imshow("Reducao de resolucao espacial - Comparativo", comparison);

    cv::waitKey(0);
}

void exercise8(const string& imagePath) {
    Mat image;
    if (!loadImage(imagePath, image)) return;

    Mat gray_image = toGray(image);

    Mat reduced_image = reducing(gray_image);
    Mat restored_image = restoreSize(reduced_image, gray_image.size());

    Mat quantized_image = quantization(gray_image, 16);

    reduced_image = reducing(gray_image);
    Mat restored_image2 = restoreSize(reduced_image, gray_image.size());
    Mat combined_image = quantization(restored_image2, 16);

    cv::putText(restored_image, "Amostragem", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2);
    cv::putText(quantized_image, "Quantizacao", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2);
    cv::putText(combined_image, "Combinado", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2);

    Mat comparison;
    cv::hconcat(std::vector<Mat>{restored_image, quantized_image, combined_image}, comparison);
    cv::namedWindow("Comparacao Amostragem vs Quantizacao", cv::WINDOW_NORMAL);
    cv::imshow("Comparacao Amostragem vs Quantizacao", comparison);

    cout << "\nComparacao visual esperada:\n";
    cout << "- Amostragem reduzida: perde detalhes finos e bordas ficam menos definidas.\n";
    cout << "- Quantizacao reduzida: mantem detalhes espaciais, mas perde suavidade tonal.\n";
    cout << "- Combinado: perde detalhes e tons, ficando mais 'degradado'.\n";

    cv::waitKey(0);
}

void printMenu() {
    cout << "\n=============================================\n";
    cout << " Laboratório 1 - Curso de Visão Computacional\n";
    cout << "=============================================\n";
    cout << "1 - Leitura e exibicao de imagem\n";
    cout << "2 - Conversao para escala de cinza\n";
    cout << "3 - Leitura do valor de um pixel\n";
    cout << "4 - Negativo da imagem\n";
    cout << "5 - Binarizacao com limiar fixo\n";
    cout << "6 - Reducao de niveis de cinza (quantizacao)\n";
    cout << "7 - Reducao de resolucao espacial\n";
    cout << "8 - Comparacao entre amostragem e quantizacao\n";
    cout << "0 - Sair\n";
    cout << "Escolha uma opcao: ";
}

int main(int argc, char** argv) {
    string imagePath;

    if (argc >= 2) {
        imagePath = argv[1];
    } else {
        cout << "Informe o caminho da imagem: ";
        std::getline(cin, imagePath);
    }

    int option = -1;
    bool running = true;

    while (running) {
        printMenu();
        cin >> option;

        switch (option) {
            case 0: 
            {
                cout << "Encerrando\n"; 
                running = false; 
            }
            break;
            case 1: 
            {
                exercise1(imagePath);
            }
            break;
            case 2:
            {
                exercise2(imagePath);
            }
            break;
            case 3: 
            {
                exercise3(imagePath);
            }
            break;
            case 4: 
            {
                exercise4(imagePath);
            }
            break;
            case 5: 
            {
                exercise5(imagePath);
            }
            break;
            case 6: 
            {
                exercise6(imagePath);
            }
            break;
            case 7: 
            {
                exercise7(imagePath);
            }
            break;
            case 8: 
            {
                exercise8(imagePath);
            }
            break;
            default:
            {
                cout << "Opcao invalida.\n";
            }
            break;
        }

        cout << "\nPressione ENTER para continuar...";
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cin.get();
        cv::destroyAllWindows();
    }

    return 0;
}