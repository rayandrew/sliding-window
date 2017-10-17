# sliding-window
Sliding Window implementation

## Overview

1. [Install prerequisites](#prerequisites)

    Before installing project make sure the following prerequisites have been met.

2. [Project Tree](#project-tree)

    Directory structure of this project

3. [Install and running the project](#installing)

    We’ll download the code from its repository on GitHub.

4. [Answers](#answers)

	Answer for questions in specification

5. [Authors and Credits](#authors)

    See men behind the project and other people that contribute to this project

---

### Prerequisites

What things you need to install the software and how to install them

Installed :
- `GNU Make` (https://www.gnu.org/software/make/)
- `g++` (https://gcc.gnu.org/)
---

### Project tree

```sh
.
├── Makefile
├── README.md
├── data
└── src
```
---

### Installing

This project is intended to run on Linux/Mac workspace.

```
git clone https://github.com/rayandrews/sliding-window.git
cd sliding-window
make
```

### Running
- For Receiving File: `./recvile`
- For Sending File: `./sendfile`
---

### Answers
1. Jika advertised window yang dikirim bernilai 0, maka menunjukkan bahwa buffer receiver telah penuh.
Cara menanganinya adalah dengan menyimpan frame yang akan dikirim dan menunggu 
hingga buffer receiver mampu menampung frame baru yang akan dikirimkan sender.

2. TCP Header
<table>
	<tr align="center">
		<td colspan="2">Source Port (16 bits)</td>
		<td colspan="2">Destination Port (2 bits)</td>
	</tr>
	<tr align="center">
		<td colspan="4">Sequence Number (32 bits)</td>
	</tr>
	<tr align="center">
		<td colspan="4">Acknowledgment Number (32 bits)</td>
	</tr>
	<tr align="center">
		<td>Data offset</td>
		<td>Reserved</td>
		<td>TCP Flag</td>
		<td colspan="2" rowspan="2">Window Size (16 bits)</td>
	</tr>
	<tr align="center">
		<td>(4 bits)</td>
		<td>(3 bits)</td>
		<td>(9 bits)</td>
	</tr>
	<tr align="center">
		<td colspan="2">Checksum (16 bits)</td>
		<td colspan="2">Urgent Pointer (16 bits)</td>
	</tr>
	<tr align="center">
		<td colspan="4">Options (0 or 32 bits optional)</td>
	</tr>
</table>

<!--

Raw table

|          Source Port (16 bits)          |          Destination Port (2 bits)           |
------------------------------------------------------------------------------------------
|                                 Sequence Number (32 bits)                              |
------------------------------------------------------------------------------------------
|                              Acknowledgment Number (32 bits)                           |
------------------------------------------------------------------------------------------
| Data offset | Reserved |    TCP Flags   |           Window Size (16 bits)              |
|  (4 bits)   | (3 bits) |     (9 bits)   |                                              |
------------------------------------------------------------------------------------------
|            Checksum (16 bits)           |          Urgent Pointer (16 bits)            |
------------------------------------------------------------------------------------------
|                             Options (0 or 32 bits optional)                            |

-->

* Source Port: Port sender
* Destination Port: Port receiver
* Sequence Number: Sequence number dari oktet data pertama di dalam segment (kecuali terdapat SYN). Jika terdapat SYN, dan sequence number merupakan initial sequence number (ISN), maka oktet data pertama adalah ISN+1.
* Acknowledgment Number: Merupakan segment yang dibutuhkan oleh segmen TCP dengan flag ACK diset bernilai 1, dan mengindikasi oktet dalam byte yang diharapkan diterima oleh pengirim dari pengirim di pengiriman berikutnya.
* Data Offset: 4 bit yang menunjukkan size dari TCP Header dalam 32 bit words. Mengindikasi di offset berapa data dimulai dan panjang dari TCP Header merupakan kelipatan 32 bit.
* Reserved: Untuk keperluan ke depannya dan harus diset dengan nilai nol.
* TCP Flags: Mengandung 9 flag dengan ukuran 1 bit
	- NS  : ECN-nonce concealment protection (merupakan flag eksperimental)
	- CWR : diset jika menerima packet yang mempunyai ECE flag dan sudah memiliki congestion control mechanism. 
	- ECE : ECN Capable jika SYN bernilai 1, apabila bernilai 0 mengindikasi terjadinya kemacetan di network.
	- URG : mengindikasi diperlukannya field urgent
	- ACK : mengindikasi diperlukannya acknowledgment, semua packet setelah initial packet dengan SYN flag membutuhkan flag ini
	- PSH : push buffered data ke aplikasi penerima
	- RST : mereset koneksi
	- SYN : untuk menandakan paket pertama yang dikirim dan diterima
	- FIN : menandakan paket terakhir yang dikirim sender
* Window Size : Berisi size dari receive window
* Checksum : Berisi 16 bit yang digenerate dari kumpulan field diatas dan digunakan untuk error-checking
* Urgent Pointer : Bila URG flag diset, maka field ini berisi offset dari sequence number yang menunjuk ke last byte dari urgent data
* Options : Size dari option ditentukan oleh data offset dan memiliki hingga 3 buah field yaitu Option-Kind, Option-Data, dan Option-Length
---
## Authors

* **Jonathan Christopher [13515001]** - [nathanchrs](https://github.com/nathanchrs)
* **Winarto [13515061]** - [yowinarto](https://github.com/yowinarto)
* **Ray Andrew [13515073]** - [rayandrews](https://github.com/rayandrews)