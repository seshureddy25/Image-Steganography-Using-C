# 🔐 Image Steganography Using C

## 📌 About the Project

**Image Steganography Using C** is a project that hides secret information inside a BMP image without significantly changing its visual appearance.

The project uses the **Least Significant Bit (LSB) technique** to encode secret messages into image pixels and retrieve them through a decoding process.

This project demonstrates practical applications of C programming, file handling, bitwise operations, and image data manipulation.

## ✨ Features

* 🔒 **Secret Data Encoding:** Hide secret text inside BMP images.
* 🔓 **Data Decoding:** Extract hidden messages from encoded images.
* 🖼️ **BMP Image Processing:** Work with bitmap image files.
* 📂 **File Handling:** Read and write image and text files using C.
* ⚙️ **Bitwise Operations:** Use bit manipulation for LSB encoding.
* 🛡️ **Validation:** Check input files and encoding requirements.

## 🛠️ Technologies Used

* **Language:** C
* **Concepts:** Pointers, Structures, File Handling, Bitwise Operations
* **Technique:** Least Significant Bit (LSB) Steganography
* **Image Format:** BMP
* **Tools:** GCC, VS Code

## 📁 Project Structure

```text
Image-Steganography-Using-C/
│
├── main.c
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── common.h
├── types.h
│
├── beautiful.bmp
├── secret.txt
├── stego.bmp
│
└── README.md
```

## ⚙️ How It Works

### 🔹 Encoding

1. Read the source BMP image.
2. Read the secret text file.
3. Extract image data and prepare it for encoding.
4. Embed the secret message into the image using LSB manipulation.
5. Generate a new stego image containing the hidden message.

### 🔹 Decoding

1. Read the encoded BMP image.
2. Extract the embedded data from the image pixels.
3. Reconstruct the hidden message.
4. Save or display the decoded secret information.

## 🚀 Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/seshureddy25/Image-Steganography-Using-C.git
```

### 2. Navigate to the Project Directory

```bash
cd Image-Steganography-Using-C
```

### 3. Compile the Project

```bash
gcc main.c encode.c decode.c -o stego
```

### 4. Run the Program

```bash
./stego
```

Follow the program's menu or command-line prompts to perform encoding and decoding.

## 📚 Concepts Learned

* File handling using `fopen()`, `fread()`, `fwrite()` and `fclose()`
* Bitwise operations and bit manipulation
* Pointers and structures in C
* Image file data handling
* Modular programming using header and source files
* Data encoding and decoding techniques
* Error handling and validation

## 🎯 Project Objective

The main objective of this project is to understand how digital information can be hidden inside image files using low-level C programming techniques.

It also provides hands-on experience with file operations, binary data manipulation, and modular software development.

## 🔮 Future Improvements

* Support for additional image formats such as PNG.
* Support for hiding larger files.
* Add encryption before encoding secret messages.
* Improve error handling and input validation.
* Develop a graphical user interface.

## 👨‍💻 Author

**Seshu Kumar Reddy**

* GitHub: [@seshureddy25](https://github.com/seshureddy25)
* Project Repository: [Image Steganography Using C](https://github.com/seshureddy25/Image-Steganography-Using-C)

---

⭐ If you find this project useful, consider giving it a star!
