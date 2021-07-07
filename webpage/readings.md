## Leitura mais recente

<center>
<h5 name="leitura"></h5>
</center>

<script>
    setInterval(function () {
        get_result_from_route("/get/readings", function(result){
            const obj = JSON.parse(result);
            var leitura_h5 = document.getElementsByName("leitura")[0];
            leitura_h5.innerText = obj.litros;
        });
    }, 1000);
</script>
