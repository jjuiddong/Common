//
// 2021-02-13, jjuiddong
// utility function library
//


export default class Util {

    // return client name
    // clusterName: clientName-cluster
    // clientName: WebBrowser-author-number
    static getClientName(clusterName) {
        let toks = clusterName.split('-')
        if (toks.length < 4)
            return ''
        return `${toks[0]}-${toks[1]}-${toks[2]}`
    }

    // return author name
    // clientName: WebBrowser-author-number
    static getAuthorName(clientName) {
        let toks = clientName.split('-')
        if (toks.length < 3)
            return ''
        return toks[1]
    }

    // deep copy string
    static copyString(str) {
		if (!str)
			return ''
		if (str.length == 0)
			return ''
		return (' ' + str).slice(1)
    }


}
