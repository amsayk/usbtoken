Spine = require('spine')
Façade = require('lib/façade')
Token = require('models/token')

class ChangePIN extends Spine.Controller

  className: 'v-scroll changepin'

  # args(doAction, app, minLength, maxLength)
  constructor: (args) ->
    super

    @bind 'release', => 
      delete @app

    @doAction.err = =>
      @oldpin.val('')
      @pin.val('')
      @pin_confirm.val('')
      @submitBtn.enable() 
      @oldpin[0].focus()      

    @bind 'setpin-error', (msg) => @app.alert(msg: msg, closable: true)    

  events:
    'submit form.changepin'   :    'submit'
    'click  form .cancel'     :    'cancel'

  elements:
    '[name=oldpin]'        :    'oldpin'   
    '[name=pin]'           :    'pin' 
    '[name=pin_confirm]'   :    'pin_confirm'
    '[type=submit]'  :    'submitBtn'

  @templ: require('views/changepin')

  render: ->
    @html ChangePIN.templ()

  cancel: ->
    @navigate '#/keys'

  params: ->

    cleaned = (key) =>
      (@[key].val() or '').trim()

    oldpin       : cleaned 'oldpin'      
    pin          : cleaned 'pin'      
    pin_confirm  : cleaned 'pin_confirm'      

  submit: (e) -> 
    @log '@actn'
    e.preventDefault()
    @submitBtn.enable(false)

    params = @params()

    if msg = @valid(params.oldpin, params.pin, params.pin_confirm)
      @app.alert(msg: msg, closable: true)
      @doAction.err()
      return false

    # doAction process
    df = app.Loading()
    @delay (-> @doAction(params).done df)

    false
  
  # private

  valid: (oldpin, pin, pin_confirm) -> 

    return app.$T('old_invalid_pin_length').Format(@minLength, @maxLength) unless oldpin.length >= @minLength and oldpin.length <= @maxLength
    
    return app.$T('new_invalid_pin_length').Format(@minLength, @maxLength) unless pin.length >= @minLength and pin.length <= @maxLength 
   
    return app.$T('confirmation_dont_match') unless pin is pin_confirm 
    
module.exports = ChangePIN