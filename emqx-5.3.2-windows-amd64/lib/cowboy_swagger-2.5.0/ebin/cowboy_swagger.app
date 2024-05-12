{application,cowboy_swagger,
             [{description,"Swagger for Cowboy Erlang projects"},
              {vsn,"2.5.0"},
              {applications,[kernel,stdlib,ranch,cowboy,trails,jsx]},
              {modules,[cowboy_swagger,cowboy_swagger_handler,
                        cowboy_swagger_json_handler,
                        cowboy_swagger_redirect_handler]},
              {registered,[]},
              {licenses,["Apache 2.0"]},
              {links,[{"Github","https://github.com/inaka/cowboy-swagger"},
                      {"Blog",
                       "http://inaka.net/blog/2015/08/19/cowboy-swagger/"},
                      {"Example",
                       "https://github.com/inaka/cowboy-swagger/tree/master/example"}]},
              {build_tools,["rebar3"]}]}.