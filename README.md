# sliding-window
Sliding Window implementation

## Overview

1. [Install prerequisites](#prerequisites)

    Before installing project make sure the following prerequisites have been met.

2. [Project Tree](#project-tree)

    Directory structure of this project

3. [Install and running the project](#installing)

    We’ll download the code from its repository on GitHub.

4. [How it works](#how-it-works)

	How sendfile and recvfile works

5. [Answers](#answers)

	Answer for questions in specification

6. [Authors and Credits](#authors)

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
- For Receiving File: `./recvile <filename> <windowsize> <buffersize> <port>`
- For Sending File: `./sendfile <filename> <windowsize> <buffersize> <destination_ip> <destination_port>`
---

### How it works
* **Sendfile**
	1. Ketika program sendfile dijalankan, input dari user akan diterima dan program akan membentuk suatu koneksi baru. 
	2. Setelah itu, program akan membaca file inputan user dan memulai pengiriman file secara byte per byte. 
	3. Program akan mengeksekusi fungsi **send_data**, yang dilakukan fungsi ini pertama kali adalah mengisi send buffer dengan data dari file.
	4. Setelah send buffer berisi, program mengambil data dari buffer dan menambahkan header-header yang diperlukan seperti sequence number, checksum, SOH, STX, ETX, dan data.
	5. Program akan mengirimkan byte yang diizinkan oleh kapasitas advertise window ke socket dengan ip tujuan dan port tujuan.
	6. Program menunggu ack dari receiver yang berisi next sequence.
	7. Setelah ack diterima akan dilakukan error-checking dengan checksum, jika tidak terjadi error, maka program akan membaca isi ack dan mengirimkan data dari buffer dengan sequence berikutnya. Jika terjadi error, maka akan ack ditolak.
	8. Jika terjadi timeout sebelum ack diterima, maka program akan mengirim ulang packet yang belum mendapat ack.
	9. Jika advertise window bernilai nol, maka akan diset menjadi satu untuk mengizinkan periodical probes.
	10. Setelah semua data berhasil ditransmisikan, akan dikirimkan sebuah paket yang menandakan akhir dari transmisi.


* **Recvfile**
	1. Ketika program recvfile dijalankan, akan dibuat koneksi baru dari inputan user.
	2. Program akan mengeksekusi fungsi **recv_data** yang menunggu transmisi dari semua alamat.
	3. Ketika packet diterima, dilakukan pengecekan apakah packet EOT. Jika iya, maka program telah selesai dan jika tidak, maka program akan melanjutkan pembacaan.
	4. Packet yang diterima akan disimpan dalam buffer receiver.
	5. Program akan mengirimkan ack ke pengirim yang memiliki sequence number berikutnya dari packet yang dikirim.
	6. Program mengambil data dari buffer receiver dan menghapus buffer tersebut serta menuliskan data tersebut ke dalam file.

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
	- Packet, Send_connection, sendfile, socket
* **Winarto [13515061]** - [yowinarto](https://github.com/yowinarto)
	- AckPacket, Recv_connection, Checksum, Utils
* **Ray Andrew [13515073]** - [rayandrews](https://github.com/rayandrews)
	- AckPakcet, Recv_connection, Logger, recvfile