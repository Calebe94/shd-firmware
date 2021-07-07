function get_result_from_route(route, callback)
{
    var request = new XMLHttpRequest();
    request.open('GET', route, true);

    request.onload = function() 
    {
        if (request.status >= 200 && request.status < 400) 
        {
            // Success!
            callback(request.responseText);
        }
        else 
        {
            // We reached our target server, but it returned an error
        }
    };

    request.onerror = function() {
    // There was a connection error of some sort
    };

    request.send();
    return false;

}

function check_id_input_field()
{
    get_result_from_route("/get/id", function(result){
        const obj = JSON.parse(result);
        var input_id = document.getElementsByName("id")[0]; 
        input_id.placeholder = obj.id;
    });
}

function check_mode_input_field()
{
    get_result_from_route("/get/mode", function(result){
        const obj = JSON.parse(result);
        if(obj.mode === "master")
        {
            var input_id = document.getElementById("master");
            input_id.checked = true;
        }
        else
        {
            var input_id = document.getElementById("slave");
            input_id.checked = true;
        }
    });
}
