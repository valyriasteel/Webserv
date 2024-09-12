# Webserv #
===================

## **Client-server ilişkisi:** 
Client-Server İlişkisi, bir bilgisayar ağı içinde, iki taraf arasındaki iletişimi tanımlayan bir modeldir. Bu modelde, "client" (istemci) ve "server" (sunucu) olarak adlandırılan iki taraf bulunur:

**Client (İstemci):**
İstemci, genellikle bir kullanıcı tarafından kontrol edilen, sunucudan belirli bir hizmet veya kaynak talep eden cihaz veya yazılımdır.
- Örneğin, bir web tarayıcısı (Chrome, Firefox gibi) bir istemci olarak hareket eder. Kullanıcı bir web sitesini ziyaret ettiğinde, tarayıcı bu siteye dair talep edilen verileri sunucudan ister.

**Server (Sunucu):**
Sunucu, istemcilerden gelen talepleri karşılayan ve gerekli verileri veya hizmetleri sağlayan cihaz veya yazılımdır.
- Örneğin, bir web sunucusu (Apache, NGINX gibi) bir sunucu olarak hareket eder. Web tarayıcısından gelen bir talebe yanıt olarak, talep edilen web sayfasını ve ilgili verileri tarayıcıya geri gönderir.

**Client-Server İlişkisi Nasıl Çalışır?**

**1. İstek (Request) Gönderme:**
İstemci, belirli bir kaynak veya hizmet talep ettiğinde, bir "request" (istek) gönderir. Bu istek, genellikle HTTP protokolü üzerinden gönderilir ve içeriğinde istenen kaynağın adresi (URL) bulunur.

**2. İstek Sunucuya Ulaşır:**
İstemcinin isteği, sunucuya iletilir. Sunucu, bu isteği alır ve istemcinin ne talep ettiğini analiz eder. Bu analiz, URL, HTTP metodu (GET, POST, vb.), başlık bilgileri gibi detayları içerir.

**3. Sunucunun Yanıt Hazırlaması:**
Sunucu, istemciden gelen isteğe uygun bir yanıt hazırlamaya başlar. Bu, bir HTML sayfası, bir dosya, bir API yanıtı veya diğer herhangi bir veri olabilir.
Sunucu ayrıca, isteğe yanıt olarak bir HTTP durum kodu (örneğin 200 OK, 404 Not Found gibi) da gönderir. Bu kod, istemcinin isteğinin başarıyla karşılanıp karşılanmadığını belirtir.

**4. Yanıtın Gönderilmesi:**
Sunucu, hazırladığı yanıtı istemciye geri gönderir. Bu yanıt, HTTP yanıtı olarak bilinir ve içerdiği veri, istemcinin talebine göre değişir.

**5. İstemcinin Yanıtı Alması:**
İstemci, sunucudan gelen yanıtı alır ve bu yanıtı işler. Örneğin, bir web tarayıcısı bu yanıtı alıp kullanıcıya web sayfası olarak gösterir.

**Örnek:**

- Kullanıcı web tarayıcısına "www.example.com" adresini yazar ve enter tuşuna basar.
- Tarayıcı (istemci) bu adresi bir HTTP GET isteği olarak sunucuya gönderir.
- Sunucu, bu isteği alır, www.example.com sayfasını bulur ve HTML içeriğini hazırlar.
- Sunucu, hazırlanan bu HTML sayfasını bir HTTP yanıtı olarak istemciye geri gönderir.
- Tarayıcı, bu yanıtı alır ve kullanıcıya web sayfasını gösterir.

client ve server arasındaki ilişki ağ bağlantısı kurulması ve iletişim protekolleriyle gerçekleşiyor.
Bu ağ bağlantısının kurulması genellikle soket (socket) oluşturulmasıyla gerçekleştirilir. Soketler, 
istemci ve sunucu arasında veri alışverişini sağlamak için kullanılan temel mekanizmalardır. Soketler, ağ 
üzerindeki iki cihazın birbirleriyle iletişim kurmasına olanak tanır.

## **Socket():**

**Soket Nedir?**
Soket, bir IP adresi ve bir port numarası ile ilişkilendirilmiş bir uç noktadır. İstemci ve sunucu arasındaki iletişimin başlangıç noktasıdır.
Bir soket, istemcinin bir sunucuya veya sunucunun istemciye veri gönderebilmesi için açılan bir kanal gibidir.
Socket'lar, TCP/IP gibi protokolleri kullanarak iletişim kurmak için kullanılır. Her bir socket, belirli bir IP adresi ve port numarası ile ilişkilendirilir.

**Soket Türleri:**

**1. Stream Sockets (TCP Sockets):**
- TCP protokolü üzerinden bağlantı odaklı iletişim sağlar. Bu tür soketler, güvenilir ve sıralı veri iletimi için kullanılır.
- Örneğin, bir web tarayıcısının bir web sunucusundan veri alması bu tür bir socket ile yapılır.

**2. Datagram Sockets (UDP Sockets):**
- UDP protokolü üzerinden bağlantısız iletişim sağlar. Bu tür soketler, veri iletiminde hız gerektiren ancak güvenilirlik gerektirmeyen durumlar için kullanılır.
- Veriler paketler (datagramlar) halinde gönderilir ve alıcıya ulaşması garanti edilmez. Paketler sırasız gelebilir veya kaybolabilir.
- Örneğin, çevrimiçi oyunlar veya video akışı gibi hızın önemli olduğu durumlarda kullanılır.

**SOCKET OLUŞTURMA ADIMLARI:**

**Sunucu Tarafında Soket Oluşturma:**
1. Soket Oluşturma (socket()): Sunucu, ilk olarak bir soket oluşturur. Bu soket, belirli bir IP adresine ve porta bağlanacak şekilde yapılandırılır.(Bu socket, socket() sistem çağrısıyla oluşturulur ve protokol tipi (TCP veya UDP) belirtilir.)
2. Bağlama (bind()): Oluşturulan soket, sunucunun IP adresine ve belirli bir porta bağlanır. Bu, istemcilerin bu IP adresi ve port üzerinden sunucuya ulaşmasını sağlar.
3. Dinleme (listen()): Sunucu, soketi dinleme moduna alır ve bu sayede istemcilerden gelen bağlantı taleplerini bekler.
4. Kabul Etme (accept()): Bir istemci, sunucuya bağlantı talebinde bulunduğunda, sunucu bu bağlantıyı kabul eder ve istemciyle iletişim kurmak için yeni bir soket oluşturur.

**İstemci Tarafında Soket Oluşturma:**
1. Soket Oluşturma (socket()): İstemci, sunucuyla iletişim kurmak için bir soket oluşturur.
2. Bağlantı Kurma (connect()): İstemci, sunucunun IP adresi ve port numarasını kullanarak sunucuya bağlanır.

**Veri İletişimi:**
1. Veri Gönderme (send()): Bağlantı kurulduktan sonra, istemci veya sunucu veriyi göndermek için soketi kullanır.
2. Veri Alma (recv()): Karşı taraf tarafından gönderilen veri, soket aracılığıyla alınır.

**Bağlantının Sonlandırılması:**
Soketi Kapatma (close()): Veri alışverişi tamamlandıktan sonra, soket kapatılır ve bağlantı sonlandırılır.

**Örnek Senaryo:**

Sunucu:
- Sunucu, socket() ile bir soket oluşturur.
- bind() ile soketi kendi IP adresine ve bir port numarasına bağlar.
- listen() komutuyla bu soketi dinlemeye başlar.
- İstemciden bir bağlantı talebi geldiğinde accept() ile bu talebi kabul eder ve istemciyle iletişim kurmak için yeni bir soket oluşturur.

İstemci:
- İstemci, socket() ile bir soket oluşturur.
- connect() ile bu soketi sunucunun IP adresine ve port numarasına bağlar.
- Bağlantı kurulduktan sonra veri alışverişi yapılır (send() ve recv() kullanılarak).
- Bu süreç, istemci ve sunucu arasındaki temel iletişim modelidir ve ağ bağlantılarının büyük çoğunluğu bu şekilde kurulur ve yönetilir. Soketler, özellikle ağ programlamasında ve dağıtık sistemlerde kritik bir rol oynar.

**uint16_t htons(uint16_t hostshort);:** htons() fonksiyonu, "host to network short" anlamına gelir ve bir tamsayıyı (genellikle bir port numarasını) host byte order'dan network byte order'a (big-endian) çevirir. Bu dönüşüm, ağ üzerindeki veri iletiminde farklı bilgisayar sistemlerinin uyumlu bir şekilde iletişim kurmasını sağlar.( Port numarasını network byte order'a çevir)

**int socket(int domain, int type, int protocol) :** Bir socket tanımlayıcısı oluşturur. Bu tanımlayıcı, ağ bağlantılarını kurmak ve yönetmek için kullanılır.
Parametreler:
1. domain: Hangi adres ailesinin kullanılacağını belirtir.
    AF_INET: IPv4 adresleri için.
    AF_INET6: IPv6 adresleri için.
2. type: Socket türünü belirtir.
    SOCK_STREAM: TCP bağlantısı için (güvenilir, bağlantı tabanlı veri iletimi).
    SOCK_DGRAM: UDP bağlantısı için (bağlantısız, datagram tabanlı iletim).
3. protocol: Kullanılacak belirli protokolü belirtir. Genellikle 0 olarak bırakılır, bu durumda varsayılan protokol seçilir (örneğin, TCP için IPPROTO_TCP, UDP için IPPROTO_UDP).

**int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);***: Socket'i belirli bir IP adresine ve port numarasına bağlar.
1. sockfd: Bağlanacak socket'in tanımlayıcısı (genellikle socket() fonksiyonu ile elde edilir).
2. addr: Bağlanacak adresi içeren sockaddr yapısının işaretçisi.
3. addrlen: Adres yapısının boyutu.

*int listen(int sockfd, int backlog);* : Bir socket'i gelen bağlantıları dinlemeye ayarlar.
1. sockfd: Dinleme yapılacak socket'in tanımlayıcısı.
2. backlog: Bağlantı kuyruğunda bekleyebilecek maksimum bağlantı sayısı.

**int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);** : gelen bir bağlantıyı kabul eder.
1. sockfd: Gelen bağlantıları dinleyen socket'in tanımlayıcısı.
2. addr: Bağlantı kurulan istemcinin adres bilgilerini içeren sockaddr yapısının işaretçisi.
3. addrlen: addr yapısının boyutunu belirten bir işaretçi.

*ssize_t recv(int sockfd, void *buf, size_t len, int flags);* : Bir socket üzerinden veri alır.
1. sockfd: Verinin alındığı socket'in tanımlayıcısı.
2. buf: Alınan verinin depolanacağı bellek alanının işaretçisi.
3. len: Alınacak maksimum veri miktarı (buf'un boyutu).
4. flags: Ekstra seçenekler için kullanılan bayraklar. Genellikle 0 olarak bırakılır.

*ssize_t send(int sockfd, const void *buf, size_t len, int flags);* : Bir socket üzerinden veri gönderir.
1. sockfd: Verinin gönderileceği socket'in tanımlayıcısı.
2. buf: Gönderilecek verinin depolandığı bellek alanının işaretçisi.
3. len: Gönderilecek veri miktarı (buf'un boyutu).
4. flags: Ekstra seçenekler için kullanılan bayraklar. Genellikle 0 olarak bırakılır.

- - - - 

**HTTP PROTEKOLÜ**

HTTP (Hypertext Transfer Protocol), internet üzerindeki veri iletişimini sağlayan temel bir uygulama protokolüdür. Web tarayıcıları (istemciler) ve web sunucuları (sunucular) arasındaki iletişimi yönetir. HTTP, tarayıcınızın bir web sitesine erişmek istediğinde, sunucuya bir istek gönderdiği ve sunucunun bu isteği yanıtladığı bir protokoldür.

*HTTP'nin Temel Yapısı:*

1. İstemci-Sunucu Modeli:
HTTP, istemci-sunucu modeline dayalıdır. İstemci, bir web tarayıcısı gibi bir uygulamadır ve sunucuya bir istek gönderir. Sunucu, bu isteği işler ve istemciye bir yanıt döner.
İstemcinin isteği, sunucudan belirli bir kaynağın (örneğin bir web sayfası, resim veya video) alınmasını talep eder.

2. Bağlantısız Protokol:
HTTP, bağlantısız (stateless) bir protokoldür. Bu, her isteğin bağımsız olduğunu ve sunucunun önceki istekler hakkında bir bilgi tutmadığını ifade eder. Her istek, baştan sona yeni bir bağlantı gibi ele alınır.

3. Metin Tabanlı Protokol:
HTTP, insanlar tarafından okunabilir metin tabanlı bir protokoldür. İstekler ve yanıtlar, basit metin satırlarından oluşur. Bu satırlarda, hangi işlem yapılacağı ve hangi verilerin iletileceği belirtilir.

*HTTP İstek ve Yanıtlarının Yapısı:*

*HTTP İsteği (HTTP Request)*
Bir HTTP isteği, istemcinin sunucuya gönderdiği bilgileri içerir. Bir HTTP isteği üç ana bileşenden oluşur:

1. İstek Satırı (Request Line):
İstek metodunu (GET, POST, DELETE, vb.), istenen kaynağın URL'sini ve HTTP versiyonunu belirtir.
Örnek: GET /index.html HTTP/1.1

2. Başlıklar (Headers):
İstek hakkında ek bilgiler sağlar. Örneğin, istemcinin hangi tarayıcıyı kullandığı (User-Agent), kabul edilen içerik türleri (Accept), ve oturum bilgileri (Cookie) gibi.
Örnek: Host: www.example.com

3. İçerik (Body):
Özellikle POST veya PUT gibi metotlarla yapılan isteklerde bulunur. İstemcinin sunucuya göndermek istediği veri bu bölümde yer alır (örneğin, bir form verisi).

Örnek bir HTTP isteği:
GET /index.html HTTP/1.1
Host: www.example.com
User-Agent: Mozilla/5.0
Accept: text/html

*HTTP Yanıtı (HTTP Response)*
Bir HTTP yanıtı, sunucunun istemciye gönderdiği bilgileri içerir. Bir HTTP yanıtı üç ana bileşenden oluşur:

1. Durum Satırı (Status Line):
HTTP versiyonu, durum kodu ve durum mesajını içerir. Durum kodları, isteğin sonucunu belirtir (örneğin, 200 OK, 404 Not Found, 500 Internal Server Error).
Örnek: HTTP/1.1 200 OK

2. Başlıklar (Headers):
Sunucu hakkında ek bilgiler sağlar. Örneğin, döndürülen içeriğin türü (Content-Type), içerik uzunluğu (Content-Length), ve sunucu yazılımı (Server) gibi.
Örnek: Content-Type: text/html

3. İçerik (Body):
Sunucunun döndürdüğü veriler burada bulunur. Bu, bir web sayfası, bir resim, JSON verisi veya başka bir türde içerik olabilir.

örnek bir HTTP yanıtı:
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 1234

<html>
  <body>
    <h1>Hello, World!</h1>
  </body>
</html>

*HTTP METHDLARI:*
1. GET: Sunucudan belirtilen kaynağı talep eder. GET istekleri sadece veriyi almak içindir, sunucuda veri değişikliği yapmaz.
2. POST: İstemciden sunucuya veri gönderir. Genellikle form verilerini göndermek için kullanılır.
3. PUT: Belirtilen kaynağı sunucuda günceller veya yaratır.
4. DELETE: Sunucudaki belirtilen kaynağı siler.
5. HEAD: GET isteği gibidir, ancak sadece başlıkları alır, içerik kısmı gönderilmez. Kaynağın mevcut olup olmadığını kontrol etmek için kullanılır.
6. OPTİONS: Sunucunun veya belirli bir kaynağın desteklediği HTTP metotlarını döner.

*HTTP Durum Kodları*
HTTP yanıtları, istemcinin isteğinin sonucunu belirten durum kodları ile gelir. Bu kodlar genellikle üç hanelidir:

- 1xx (Bilgilendirme): İstek alındı ve işleniyor.
- 2xx (Başarılı): İstek başarıyla alındı, anlaşıldı ve kabul edildi.
    200 OK: İstek başarıyla tamamlandı.
- 3xx (Yönlendirme): İstemcinin başka bir kaynağa yönlendirilmesi gerekir.
    301 Moved Permanently: Kaynak kalıcı olarak taşınmıştır.
- 4xx (İstemci Hatası): İstemci tarafından yapılan bir hata var.
    404 Not Found: İstenen kaynak bulunamadı.
- 5xx (Sunucu Hatası): Sunucu tarafında bir hata oluştu.
    500 Internal Server Error: Genel bir sunucu hatası.

HTTP istekleri (GET, POST, PUT, DELETE) genellikle bir web sunucusuna veya API'ye gönderilir.
Web sunucusu bu istekleri alır, işlemek için uygun bir işlem (fonksiyon) çağırır ve bu işlem, veritabanıyla etkileşim kurar.
Web sunucusu, veritabanına bağlantı kurarak istenen işlemi (veri ekleme, güncelleme, silme, alma) gerçekleştirir ve sonuçları geri döndürür.

*HTTP ve HTTPS*
- HTTP: Verilerin şifresiz olarak iletildiği bir protokoldür. Veri iletişimi düz metin olarak yapılır, bu da güvenlik riskleri oluşturur.
- HTTPS: HTTP'nin şifreli versiyonudur. Veri iletişimi SSL/TLS protokolleri ile şifrelenir, bu da güvenliği artırır. HTTPS, özellikle hassas verilerin iletildiği durumlarda kullanılır (örneğin, banka işlemleri).


**Non-blocking I/O Nedir?**
Non-blocking I/O (Giriş/Çıkış), bir işlem gerçekleştirilirken işlem yapan fonksiyonun hemen döndürülmesini sağlayan bir tekniktir. Bu, bir işlem (örneğin veri okuma veya yazma) tamamlanana kadar programın beklemesini (bloklanmasını) önler. Bu yaklaşım, özellikle yüksek performans gerektiren sunucu uygulamalarında kullanılır, çünkü sunucunun sürekli olarak erişilebilir olmasını ve aynı anda birden fazla istemciyi yönetebilmesini sağlar.

- Blocking I/O: Geleneksel (blocking) I/O işlemlerinde, bir işlem (örneğin, veri okuma veya yazma) tamamlanana kadar program o noktada durur ve başka bir işlem yapamaz. Örneğin, bir socket'ten veri okunmasını beklerken program durur ve veri gelene kadar başka hiçbir işlem yapmaz.

- Non-blocking I/O: Non-blocking I/O'da, program bir işlem talebinde bulunur (örneğin, veri okuma), ancak veri hazır değilse program o noktada durmaz; hemen döner ve başka işlemler yapmaya devam edebilir. Veri hazır olduğunda program tekrar bu veriyi işleyebilir.

- Non-blocking I/O, sunucuların aynı anda birçok istemciyle etkileşime girmesini sağlar. Örneğin, bir web sunucusu birden fazla istemciden gelen HTTP isteklerini aynı anda alabilir ve işleyebilir. Bu, sunucunun bir istemciden gelen büyük bir veri parçasını beklerken diğer istemcilerle de ilgilenebilmesini sağlar.

---------------------------------------------------------------------------------------------------------
*Poll() ve select() fonksiyonları*
poll() ve select() fonksiyonları, birden fazla dosya tanımlayıcısının (file descriptor) durumunu aynı anda kontrol etmek için kullanılır. Bu fonksiyonlar, hangi dosya tanımlayıcıların veri okumak, yazmak veya hata kontrolü yapmak için hazır olduğunu belirler

*select()* : select(), bir veya birden fazla dosya tanımlayıcının durumunu izlemek için kullanılır. Bu fonksiyon, hangi tanımlayıcıların okunabilir, yazılabilir veya hata durumunda olduğunu belirler.

*int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);*
#parametreler:
- nfds: İzlenecek dosya tanımlayıcılarının maksimum değeri + 1. (Yani en büyük dosya tanımlayıcısının değeri + 1).
- readfds: Veri okumaya hazır olan tanımlayıcılar için kontrol edilen dosya tanımlayıcı seti.
- writefds: Veri yazmaya hazır olan tanımlayıcılar için kontrol edilen dosya tanımlayıcı seti.
- exceptfds: Hata durumlarını kontrol etmek için kullanılan dosya tanımlayıcı seti.
- timeout: Bekleme süresini belirten yapı. NULL olarak geçilirse, select() süresiz olarak bekler.

*FD_SET Makroları:*

- fd_set makroları, select() fonksiyonuyla birlikte kullanılır ve belirli bir dosya tanımlayıcı kümesi 
(file descriptor set) üzerinde işlemler yapmayı sağlar. Bu makrolar, dosya tanımlayıcılarını bir küme 
içinde toplamak, kümeyi sıfırlamak, belirli bir tanımlayıcıyı kümeye eklemek veya çıkarmak gibi işlemleri 
gerçekleştirir. Bu işlemler, ağ programlamada özellikle birden fazla dosya tanımlayıcıyı (örneğin, 
socket'leri) izlemek ve hangilerinin veri iletimine hazır olduğunu belirlemek için kullanılır.

fd_set, dosya tanımlayıcılarını bir arada tutan bir veri yapısıdır. select() fonksiyonuna iletilen bu yapı, hangi dosya tanımlayıcıların izleneceğini belirtir ve fonksiyon döndüğünde hangi tanımlayıcıların aktif olduğunu gösterir. Bu yapıyı yönetmek ve üzerinde işlem yapmak için bazı makrolar kullanılır:

1. FD_SET(fd, &set) : Bir dosya tanımlayıcısını fd_set kümesine ekler.
    fd: Kümeye eklenecek dosya tanımlayıcısı (file descriptor).
    set: fd_set kümesini temsil eden bir işaretçi.
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    fd_set readfds;
    FD_SET(sockfd, &readfds);  // sockfd'yi readfds kümesine ekler


2. FD_CLR(fd, &set) : Belirli bir dosya tanımlayıcısını fd_set kümesinden çıkarır.
    fd: Kümeden çıkarılacak dosya tanımlayıcısı.
    set: fd_set kümesini temsil eden bir işaretçi.

    FD_CLR(sockfd, &readfds);  // sockfd'yi readfds kümesinden çıkarır

3. FD_ISSET(fd, &set) : Bir dosya tanımlayıcısının belirli bir fd_set kümesinde olup olmadığını kontrol eder.Bu makro, select() fonksiyonu döndükten sonra kullanılır.
    fd: Durumu kontrol edilecek dosya tanımlayıcısı.
    set: fd_set kümesini temsil eden bir işaretçi.

    if (FD_ISSET(sockfd, &readfds)) {
    // sockfd üzerinde veri okumaya hazır}

4. FD_ZERO(&set) :  Bir fd_set kümesini sıfırlar, yani küme içindeki tüm dosya tanımlayıcılarını çıkarır.
    set: Sıfırlanacak fd_set kümesini temsil eden bir işaretçi.

    FD_ZERO(&readfds);  // readfds kümesini sıfırlar, tüm tanımlayıcıları çıkarır
    Ne İşe Yarar?: Bir dosya tanımlayıcı kümesini başlatır (temizler). Bu, genellikle select() fonksiyonunu çağırmadan önce yapılan ilk işlemdir. Daha sonra bu kümeye izlenecek tanımlayıcılar eklenir.

*fd_set ve select() ile Birlikte Kullanım*
fd_set makroları, select() fonksiyonuyla birlikte kullanılır. select() fonksiyonu, belirli bir süre boyunca (veya süresiz olarak) bir veya daha fazla dosya tanımlayıcının (örneğin, socket'lerin) olaylara (okuma, yazma, hata) hazır olup olmadığını kontrol eder.

- örnek:
    ```cpp
    fd_set readfds;
    FD_ZERO(&readfds);         // Okuma kümesini başlat
    FD_SET(sockfd1, &readfds); // İlk socket'i ekle
    FD_SET(sockfd2, &readfds); // İkinci socket'i ekle

    int max_fd = sockfd2 + 1; // En büyük dosya tanımlayıcı + 1

    struct timeval tv;
    tv.tv_sec = 5;  // 5 saniye bekle
    tv.tv_usec = 0;

    int ret = select(max_fd, &readfds, NULL, NULL, &tv);

    if (ret > 0) {
        if (FD_ISSET(sockfd1, &readfds)) {
            // sockfd1 üzerinde veri okumaya hazır
        }
        if (FD_ISSET(sockfd2, &readfds)) {
            // sockfd2 üzerinde veri okumaya hazır
         }
    } else if (ret == 0) {
        // Zaman aşımı oldu, hiçbir tanımlayıcı hazır değil
    } else {
        // Bir hata oluştu
        perror("select");
    }

    Bu Örnekte:
    FD_ZERO(&readfds);: Okuma kümesini sıfırlar.
    FD_SET(sockfd1, &readfds);: sockfd1 tanımlayıcısını okuma kümesine ekler.
    FD_SET(sockfd2, &readfds);: sockfd2 tanımlayıcısını okuma kümesine ekler.
    select(max_fd, &readfds, NULL, NULL, &tv);: Belirtilen kümedeki tanımlayıcıların durumunu kontrol eder. Eğer bir tanımlayıcı olay için hazırsa (ret > 0), FD_ISSET() ile kontrol edilir.
    Sonuç
    fd_set makroları, select() fonksiyonuyla birlikte kullanılan önemli araçlardır. Bu makrolar, hangi dosya tanımlayıcıların izleneceğini belirlemek, bu tanımlayıcıların durumunu kontrol etmek ve gerektiğinde kümeleri sıfırlamak için kullanılır. Ağ programlama ve özellikle non-blocking I/O senaryolarında, birden fazla bağlantıyı etkin bir şekilde yönetmek için vazgeçilmezdir.





*******************************************************************************************
Domain isimleri, insanların web sitelerini bulmak için kullandığı isimlerdir(www.example.com gibi) ve DNS, bu isimleri bilgisayarların anlayabileceği IP adreslerine dönüştüren bir sistemdir. Bu sayede, internet üzerinde kolayca gezinebiliriz.