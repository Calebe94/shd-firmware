<center>
    <h3>Configurações</h3>
</center>

<fieldset>
    <legend>Modo de Operação:</legend>
    <form target="_top" action="/set/mode" id="mode" method="POST">
        <input type="radio" id="master" name="mode" value="master">
        <label for="master">Mestre</label><br>
        <input type="radio" id="slave" name="mode" value="slave">
        <label for="slave">Escravo</label><br>
        <input onclick="redirect();" type="submit" value="Confirmar">
    </form>
</fieldset>

<fieldset>
    <legend>ID:</legend>
    <form target="_top" action="/set/id" id="mode" method="POST">
        <input type="text" name="id" id="id" placeholder="ID de 1 à 254" pattern="([1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-4])$">
        <input onclick="redirect();" type="submit" value="Confirmar">
    </form>
</fieldset>

<fieldset id="devices-fieldset">
    <legend>Dispositivos:</legend>
    <div style="width: 100%; display: table;">
        <div style="display: table-row">
            <div style="width: 600px; display: table-cell;">
            <form target="_top" action="/add/device" id="mode" method="POST">
                <input type="text" name="device" id="device" placeholder="New Device" pattern="([1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-4])$">
                <input onclick="redirect();" type="submit" value="Confirmar">
            </form>
            </div>
            <div id="devices-div" style="display: table-cell;">
                <table id="devices-table">
                    <tr>
                        <th>Device ID</th>
                        <th>Delete</th>
                    </tr>
                </table>
            </div>
        </div>
    </div>
</fieldset>

