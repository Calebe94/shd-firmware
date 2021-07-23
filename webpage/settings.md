<center>
    <h3>Configurações</h3>
</center>

<fieldset id="device_mode_filedset">
    <legend>Modo de Operação:</legend>
    <form target="_top" action="/set/mode" id="mode" method="POST">
        <input type="radio" id="controller" name="mode" value="controller">
        <label for="controller">Controlador</label><br>
        <input type="radio" id="peripheral" name="mode" value="peripheral">
        <label for="peripheral">Periférico</label><br>
        <input onclick="redirect();" type="submit" value="Confirmar">
    </form>
</fieldset>

<fieldset id="peripheral_id_fieldset">
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

<fieldset id="phone_number_fieldset">
    <legend>Telefone:</legend>
    <form target="_top" action="/set/phone" id="phone_number" method="POST">
        <input type="tel" name="phone" id="phone" placeholder="Telefone" pattern="^\(?(?:[14689][1-9]|2[12478]|3[1234578]|5[1345]|7[134579])\)? ?(?:[2-8]|9[1-9])[0-9]{3}\-?[0-9]{4}$">
        <input onclick="redirect();" type="submit" value="Confirmar">
    </form>
</fieldset>

<fieldset id="local_fieldset">
    <legend>Endereço:</legend>
    <form target="_top" action="/set/local" id="local" method="POST">
        <input type="text" name="local" id="local_address" placeholder="Endereço">
        <input onclick="redirect();" type="submit" value="Confirmar">
    </form>
</fieldset>
